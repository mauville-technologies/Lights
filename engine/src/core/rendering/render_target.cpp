//
// Created by ozzadar on 2024-12-20.
//

#include "lights/core/rendering/render_target.h"

#include "spdlog/spdlog.h"

namespace OZZ {

    RenderTarget::RenderTarget(rendering::RHIDevice* inDevice, RenderTargetParams&& inParams)
        : device(inDevice) {
        activeParams = setupRenderTarget(inParams);
    }

    RenderTarget::~RenderTarget() {
        if (texture) {
            texture.reset();
        }
    }

    void RenderTarget::Begin(rendering::RHIFrameContext& frameContext) {
        if (activeParams.Type == RenderTargetType::Viewport) {
            auto descriptorCopy = renderPassDescriptor;
            descriptorCopy.ColorAttachmentCount = 1;
            descriptorCopy.ColorAttachments[0].Texture = frameContext.GetBackbufferImage();
            descriptorCopy.DepthAttachment = {
                .Texture = frameContext.GetBackbufferDepthImage(),
                .Load = rendering::LoadOp::Clear,
                .Store = rendering::StoreOp::DontCare,
                .Clear =
                    {
                        .Depth = 1.f,
                        .Stencil = 0,
                    },
                .Layout = rendering::TextureLayout::DepthStencilAttachment,
            };

            device->BeginRenderPass(frameContext, descriptorCopy);
            device->SetViewport(frameContext,
                                {
                                    .X = 0,
                                    .Y = 0,
                                    .Width = static_cast<float>(activeParams.Size.x),
                                    .Height = static_cast<float>(activeParams.Size.y),
                                });

            device->SetScissor(frameContext,
                               {
                                   .X = 0,
                                   .Y = 0,
                                   .Width = activeParams.Size.x,
                                   .Height = activeParams.Size.y,
                               });
            return;
        }
        renderPassDescriptor.ColorAttachments->Texture = texture->GetRHIHandle();
        renderPassDescriptor.ColorAttachmentCount = 1;
        // Transition the texture to color attachment
        if (activeParams.Type == RenderTargetType::Texture) {
            device->TextureResourceBarrier(frameContext,
                                           rendering::TextureBarrierDescriptor{
                                               .Texture = texture->GetRHIHandle(),
                                               .OldLayout = rendering::TextureLayout::Undefined,
                                               .NewLayout = rendering::TextureLayout::ColorAttachment,
                                               .SrcStage = rendering::PipelineStage::ColorAttachmentOutput,
                                               .DstStage = rendering::PipelineStage::ColorAttachmentOutput,
                                               .SrcAccess = rendering::Access::None,
                                               .DstAccess = rendering::Access::ColorAttachmentWrite,
                                           });
        }

        device->BeginRenderPass(frameContext, renderPassDescriptor);
        device->SetViewport(frameContext,
                            {
                                .X = 0,
                                .Y = 0,
                                .Width = static_cast<float>(activeParams.Size.x),
                                .Height = static_cast<float>(activeParams.Size.y),
                            });

        device->SetScissor(frameContext,
                           {
                               .X = 0,
                               .Y = 0,
                               .Width = activeParams.Size.x,
                               .Height = activeParams.Size.y,
                           });
    }

    void RenderTarget::End(rendering::RHIFrameContext& frameContext) const {
        device->EndRenderPass(frameContext);
        // Transition the texture to shader read optimal if it's a texture render target
        if (activeParams.Type == RenderTargetType::Texture) {
            device->TextureResourceBarrier(frameContext,
                                           rendering::TextureBarrierDescriptor{
                                               .Texture = texture->GetRHIHandle(),
                                               .OldLayout = rendering::TextureLayout::ColorAttachment,
                                               .NewLayout = rendering::TextureLayout::ShaderReadOnly,
                                               .SrcStage = rendering::PipelineStage::ColorAttachmentOutput,
                                               .DstStage = rendering::PipelineStage::FragmentShader,
                                               .SrcAccess = rendering::Access::ColorAttachmentWrite,
                                               .DstAccess = rendering::Access::ShaderRead,
                                               .SubresourceRange =
                                                   {
                                                       .Aspect = rendering::TextureAspect::Color,
                                                       .BaseMipLevel = 0,
                                                       .LevelCount = 1,
                                                       .BaseArrayLayer = 0,
                                                       .LayerCount = 1,
                                                   },
                                           });
        }
    }

    void RenderTarget::Resize(glm::uvec2 inSize) {
        if (inSize == activeParams.Size) {
            // don't do anything if the render target matcheTextureDescriptors
            return;
        }

        auto params = activeParams;
        params.Size = inSize;
        activeParams = setupRenderTarget(params);
    }

    RenderTargetParams RenderTarget::setupRenderTarget(const RenderTargetParams& inParams) {
        if (texture) {
            texture.reset();
        }
        renderPassDescriptor = {
            .ColorAttachments = {},
            .ColorAttachmentCount = 0,
            .DepthAttachment = {},
            .StencilAttachment = {},
            .RenderArea =
                {
                    .X = 0,
                    .Y = 0,
                    .Width = inParams.Size.x,
                    .Height = inParams.Size.y,
                },
            .LayerCount = 1,
        };

        renderPassDescriptor.ColorAttachmentCount = 1;
        renderPassDescriptor.ColorAttachments[0] = {
            // TODO: We may not want to clear all the time -- probably worth putting in params if necessary
            .Load = rendering::LoadOp::Clear,
            .Store = rendering::StoreOp::Store,
            .Clear =
                {
                    .R = activeParams.ClearColor.r,
                    .G = activeParams.ClearColor.g,
                    .B = activeParams.ClearColor.b,
                    .A = activeParams.ClearColor.a,
                },
            .Layout = rendering::TextureLayout::ColorAttachment,
        };
        renderPassDescriptor.DepthAttachment = {
            .Load = rendering::LoadOp::Clear,
            .Store = rendering::StoreOp::DontCare,
            .Clear =
                {
                    .Depth = 1.f,
                    .Stencil = 0,
                },
            .Layout = rendering::TextureLayout::DepthStencilAttachment,
        };

        if (inParams.Type == RenderTargetType::Texture) {
            texture = std::make_shared<Texture>(
                device,
                rendering::TextureDescriptor{
                    .Width = inParams.Size.x,
                    .Height = inParams.Size.y,
                    .Format = rendering::TextureFormat::RGBA8,
                    .Usage = rendering::TextureUsage::Sampled | rendering::TextureUsage::ColorAttachment,
                });
            renderPassDescriptor.ColorAttachments[0].Texture = texture->GetRHIHandle();

            depthTexture = std::make_shared<Texture>(device,
                                                     rendering::TextureDescriptor{
                                                         .Width = inParams.Size.x,
                                                         .Height = inParams.Size.y,
                                                         .Format = rendering::TextureFormat::D24S8,
                                                         .Usage = rendering::TextureUsage::DepthAttachment,
                                                     });
            renderPassDescriptor.DepthAttachment.Texture = depthTexture->GetRHIHandle();
        }

        return inParams;
    }
} // namespace OZZ
