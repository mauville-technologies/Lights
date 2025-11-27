//
// Created by paulm on 2025-10-21.
//

#include "lights/rendering/renderables/renderable_viewport.h"

#include "glm/ext/matrix_transform.hpp"
#include "lights/rendering/shapes.h"
#include "lights/rendering/vertex.h"

#include <array>

namespace OZZ {
    RenderableViewport::RenderableViewport() {}

    void RenderableViewport::Init() {
        // create the render target
        renderTarget = std::make_unique<RenderTarget>(RenderTargetParams{.Type = RenderTargetType::Viewport});
        renders.insert(std::make_pair(GetRenderableName(), renderTarget.get()));

        // TODO: @paulm -- quad to render to
        const auto quadBuffer = std::make_shared<IndexVertexBuffer>();

        auto vertices = std::array<const Vertex, 4>{Vertex{
                                                        // Bottom Left
                                                        .position = {-1.f, 1.f, 0.f},
                                                        .uv = {0.f, 1.f},
                                                    },
                                                    Vertex{
                                                        // Top Left
                                                        .position = {-1.f, -1.f, 0.f},
                                                        .uv = {0.f, 0.f},
                                                    },
                                                    Vertex{
                                                        // Top Right
                                                        .position = {1.f, -1.f, 0.f},
                                                        .uv = {1.f, 0.f},
                                                    },
                                                    Vertex{
                                                        // Bottom Right
                                                        .position = {1.f, 1.f, 0.f},
                                                        .uv = {1.f, 1.f},
                                                    }};

        auto indices = std::array<unsigned int, 6>{0, 1, 3, 2, 3, 1};
        quadBuffer->UploadData(OZZ::quadVertices, OZZ::quadIndices);

        const auto shader = std::make_shared<Shader>(VertexShader, FragmentShader, true);
        const auto material = std::make_shared<Material>();
        material->SetShader(shader);
        sceneObject = {.Transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.25)), .Mesh = quadBuffer, .Mat = material};
    }

    bool RenderableViewport::render() {
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
            renderTarget->Begin();
            glClearColor(0.f, 0.f, 0.f, 0.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            sceneObject.Mat->AddTextureMapping(
                {.SlotName = "inTexture", .SlotNumber = GL_TEXTURE0, .TextureResource = render->GetTexture()});

            auto& [transform, objMesh, objMat] = sceneObject;

            objMat->Bind();
            objMesh->Bind();
            const auto drawMode = ToGLEnum(objMat->GetSettings().Mode);
            glDrawElements(drawMode, objMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
            renderTarget->End();
            return true;
        }
        return false;
    }
} // namespace OZZ
