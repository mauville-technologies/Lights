//
// Created by paulm on 2025-10-21.
//

#include "lights/core/rendering/renderables/renderable_viewport.h"

#include "glm/ext/matrix_transform.hpp"
#include "lights/core/rendering/shapes.h"
#include "lights/core/rendering/vertex.h"

#include <array>

namespace OZZ {
    RenderableViewport::RenderableViewport() {}

    void RenderableViewport::Init(rendering::RHIDevice* inDevice) {
        device = inDevice;
        // create the render target
        renderTarget = std::make_unique<RenderTarget>(device,
                                                      RenderTargetParams{
                                                          .Type = RenderTargetType::Viewport,
                                                          .ClearColor = {1.f, 0.f, 0.f, 0.f},
                                                      });
        renders.insert(std::make_pair(GetRenderableName(), renderTarget.get()));

        // TODO: @paulm -- quad to render to
        const auto quadVertices = device->CreateBuffer({
            .Size = sizeof(Vertex) * 4,
            .Usage = rendering::BufferUsage::VertexBuffer,
            .Access = rendering::BufferMemoryAccess::CpuToGpu,
        });

        const auto quadIndices = device->CreateBuffer({
            .Size = sizeof(uint32_t) * 6,
            .Usage = rendering::BufferUsage::IndexBuffer,
            .Access = rendering::BufferMemoryAccess::CpuToGpu,
        });

        auto vertices = std::array<const Vertex, 4>{Vertex{
                                                        // Top Left (Y=+1 is top after viewport Y-flip)
                                                        .Position = {-1.f, 1.f, 0.f},
                                                        .UV = {0.f, 0.f},
                                                    },
                                                    Vertex{
                                                        // Bottom Left
                                                        .Position = {-1.f, -1.f, 0.f},
                                                        .UV = {0.f, 1.f},
                                                    },
                                                    Vertex{
                                                        // Bottom Right
                                                        .Position = {1.f, -1.f, 0.f},
                                                        .UV = {1.f, 1.f},
                                                    },
                                                    Vertex{
                                                        // Top Right
                                                        .Position = {1.f, 1.f, 0.f},
                                                        .UV = {1.f, 0.f},
                                                    }};

        constexpr auto indices = std::array<unsigned int, 6>{0, 1, 3, 2, 3, 1};
        device->UpdateBuffer(quadVertices, vertices.data(), sizeof(Vertex) * vertices.size(), 0);
        device->UpdateBuffer(quadIndices, indices.data(), sizeof(uint32_t) * indices.size(), 0);

        const auto shader = std::make_shared<OZZ::Shader>(device,
                                                          rendering::ShaderSourceParams{
                                                              .Vertex = VertexShader,
                                                              .Fragment = FragmentShader,
                                                          });
        const auto material = std::make_shared<Material>(device);
        material->SetShader(shader);
        sceneObject = {
            .Transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.25)),
            .MeshData =
                {
                    .VertexBuffer = quadVertices,
                    .IndexBuffer = quadIndices,
                    .VertexCount = vertices.size(),
                    .IndexCount = indices.size(),
                },
            .Mat = material,
        };
    }

    void RenderableViewport::Resize(const glm::uvec2 size) {
        renderTarget->Resize(size);
    }

    bool RenderableViewport::render(rendering::RHIFrameContext& frameContext) {
        // There should only be one input, the scene
        if (inputs.size() != 1) {
            spdlog::error("RenderableViewport expects exactly one input (the scene). Got {} inputs.", inputs.size());
            return false;
        }

        auto* renderNode = static_cast<Renderable*>(inputs[0]);
        auto sceneRenderOpt = renderNode->GetRender(GetRequiredInputs()[0]);
        if (!sceneRenderOpt) {
            spdlog::error("Required input {} not present.", GetRequiredInputs()[0]);
            return false;
        }

        // check the render target type
        auto render = sceneRenderOpt.value();
        if (render->GetType() != RenderTargetType::Texture) {
            spdlog::error("Required input {} is not a texture.", GetRequiredInputs()[0]);
        }

        if (renderTarget) {
            renderTarget->Begin(frameContext);
            auto attributeDescriptions = OZZ::Vertex::GetAttributeDescriptions();
            device->SetGraphicsState(frameContext,
                                     {
                                         .Rasterization =
                                             {
                                                 .Cull = OZZ::rendering::CullMode::None,
                                                 .Front = OZZ::rendering::FrontFace::CounterClockwise,
                                             },
                                         .ColorBlend = {{
                                             .BlendEnable = true,
                                             .SrcColorFactor = OZZ::rendering::BlendFactor::SrcAlpha,
                                             .DstColorFactor = OZZ::rendering::BlendFactor::OneMinusSrcAlpha,
                                             .ColorBlendOp = OZZ::rendering::BlendOp::Add,
                                             .SrcAlphaFactor = OZZ::rendering::BlendFactor::One,
                                             .DstAlphaFactor = OZZ::rendering::BlendFactor::OneMinusSrcAlpha,
                                             .AlphaBlendOp = OZZ::rendering::BlendOp::Add,
                                             .ColorWriteMask = static_cast<OZZ::rendering::ColorComponentFlags>(
                                                 OZZ::rendering::ColorComponent::R | OZZ::rendering::ColorComponent::G |
                                                 OZZ::rendering::ColorComponent::B | OZZ::rendering::ColorComponent::A),
                                         }},
                                         .ColorBlendAttachmentCount = 1,
                                         .VertexInput =
                                             {
                                                 .Bindings = {OZZ::Vertex::GetBindingDescription()},
                                                 .BindingCount = 1,
                                                 .Attributes = {attributeDescriptions[0],
                                                                attributeDescriptions[1],
                                                                attributeDescriptions[2],
                                                                attributeDescriptions[3]},
                                                 .AttributeCount = attributeDescriptions.size(),
                                             },
                                     });

            sceneObject.Mat->SetResource(0, 1, render->GetTexture()->GetRHIHandle());

            auto& [transform, mesh, objMat] = sceneObject;

            objMat->Bind(frameContext);
            device->BindBuffer(frameContext, mesh.VertexBuffer);
            device->BindBuffer(frameContext, mesh.IndexBuffer);
            device->DrawIndexed(frameContext, mesh.IndexCount, 1, 0, 0, 0);
            renderTarget->End(frameContext);
            return true;
        }
        return false;
    }
} // namespace OZZ
