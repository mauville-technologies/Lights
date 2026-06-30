//
// Created by ozzadar on 2024-12-20.
//

#include "lights/core/rendering/render_target.h"
#include "lights/core/util/profiling.h"

#include "spdlog/spdlog.h"

namespace OZZ {
    namespace {
        [[nodiscard]] rendering::TextureAspect GetTextureAspect(const rendering::TextureFormat format) {
            switch (format) {
                case rendering::TextureFormat::D32Float:
                    return rendering::TextureAspect::Depth;
                case rendering::TextureFormat::D24S8:
                    return static_cast<rendering::TextureAspect>(static_cast<int>(rendering::TextureAspect::Depth) |
                                                                 static_cast<int>(rendering::TextureAspect::Stencil));
                default:
                    return rendering::TextureAspect::Color;
            }
        }
    } // namespace

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
        OZZ_PROFILE_FUNCTION;
        if (!frameContext.IsValid())
            return;
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
            if (activeParams.bHasDepth && depthTexture) {
                device->TextureResourceBarrier(frameContext,
                                               rendering::TextureBarrierDescriptor{
                                                   .Texture = depthTexture->GetRHIHandle(),
                                                   .OldLayout = rendering::TextureLayout::Undefined,
                                                   .NewLayout = rendering::TextureLayout::DepthStencilAttachment,
                                                   .SrcStage = rendering::PipelineStage::None,
                                                   .DstStage = rendering::PipelineStage::AllGraphics,
                                                   .SrcAccess = rendering::Access::None,
                                                   .DstAccess = rendering::Access::DepthStencilAttachmentWrite,
                                                   .SubresourceRange =
                                                       {
                                                           .Aspect = GetTextureAspect(depthTexture->GetDescriptor().Format),
                                                           .BaseMipLevel = 0,
                                                           .LevelCount = 1,
                                                           .BaseArrayLayer = 0,
                                                           .LayerCount = 1,
                                                       },
                                               });
            }
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
        OZZ_PROFILE_FUNCTION;
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
            if (activeParams.bHasDepth && activeParams.bDepthShaderReadable && depthTexture) {
                device->TextureResourceBarrier(frameContext,
                                               rendering::TextureBarrierDescriptor{
                                                   .Texture = depthTexture->GetRHIHandle(),
                                                   .OldLayout = rendering::TextureLayout::DepthStencilAttachment,
                                                   .NewLayout = rendering::TextureLayout::ShaderReadOnly,
                                                   .SrcStage = rendering::PipelineStage::AllGraphics,
                                                   .DstStage = rendering::PipelineStage::FragmentShader,
                                                   .SrcAccess = rendering::Access::DepthStencilAttachmentWrite,
                                                   .DstAccess = rendering::Access::ShaderRead,
                                                   .SubresourceRange =
                                                       {
                                                           .Aspect = GetTextureAspect(depthTexture->GetDescriptor().Format),
                                                           .BaseMipLevel = 0,
                                                           .LevelCount = 1,
                                                           .BaseArrayLayer = 0,
                                                           .LayerCount = 1,
                                                       },
                                               });
            }
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
        spdlog::trace("setupRenderTarget: {}x{} hasDepth={}", inParams.Size.x, inParams.Size.y, inParams.bHasDepth);
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
                    .R = inParams.ClearColor.r,
                    .G = inParams.ClearColor.g,
                    .B = inParams.ClearColor.b,
                    .A = inParams.ClearColor.a,
                },
            .Layout = rendering::TextureLayout::ColorAttachment,
        };

        if (inParams.bHasDepth) {
            renderPassDescriptor.DepthAttachment = {
                .Load = rendering::LoadOp::Clear,
                .Store = inParams.bDepthShaderReadable ? rendering::StoreOp::Store : rendering::StoreOp::DontCare,
                .Clear =
                    {
                        .Depth = 1.f,
                        .Stencil = 0,
                    },
                .Layout = rendering::TextureLayout::DepthStencilAttachment,
            };
        }

        if (inParams.Type == RenderTargetType::Texture && inParams.Size.x > 0 && inParams.Size.y > 0) {
            texture = std::make_shared<Texture>(
                device,
                rendering::TextureDescriptor{
                    .Width = inParams.Size.x,
                    .Height = inParams.Size.y,
                    .Format = inParams.ColorFormat,
                    .Usage = rendering::TextureUsage::Sampled | rendering::TextureUsage::ColorAttachment,
                });
            renderPassDescriptor.ColorAttachments[0].Texture = texture->GetRHIHandle();

            if (inParams.bHasDepth) {
                depthTexture = std::make_shared<Texture>(device,
                                                         rendering::TextureDescriptor{
                                                              .Width = inParams.Size.x,
                                                              .Height = inParams.Size.y,
                                                              .Format = inParams.bDepthShaderReadable
                                                                            ? rendering::TextureFormat::D32Float
                                                                            : rendering::TextureFormat::D24S8,
                                                              .Usage = inParams.bDepthShaderReadable
                                                                           ? rendering::TextureUsage::Sampled |
                                                                                 rendering::TextureUsage::DepthAttachment
                                                                           : rendering::TextureUsage::DepthAttachment,
                                                              .Sampler =
                                                                  {
                                                                      .MinFilter = rendering::TextureFilter::Nearest,
                                                                      .MagFilter = rendering::TextureFilter::Nearest,
                                                                      .WrapU = rendering::TextureWrap::ClampToEdge,
                                                                      .WrapV = rendering::TextureWrap::ClampToEdge,
                                                                      .WrapW = rendering::TextureWrap::ClampToEdge,
                                                                      .GenerateMipmaps = false,
                                                                  },
                                                           });
                renderPassDescriptor.DepthAttachment.Texture = depthTexture->GetRHIHandle();
            }
        }

        return inParams;
    }
} // namespace OZZ
