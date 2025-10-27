//
// Created by ozzadar on 2024-12-20.
//

#include "lights/rendering/render_target.h"

#include "spdlog/spdlog.h"

#include <glad/glad.h>

namespace OZZ {
    RenderTarget::RenderTarget(RenderTargetParams&& inParams) {
        activeParams = setUpRenderTarget(inParams);
    }

    void RenderTarget::Begin() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    void RenderTarget::End() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderTarget::Resize(glm::ivec2 inSize) {
        if (inSize == activeParams.Size && framebuffer) {
            // don't do anything if the render target matches
            return;
        }

        auto params = activeParams;
        params.Size = inSize;
        activeParams = setUpRenderTarget(params);
    }

    RenderTargetParams RenderTarget::setUpRenderTarget(const RenderTargetParams& inParams) {
        // TODO: @paulm -- might need some sync structures here to make sure that things don't do things when things are
        // happening
        if (inParams != activeParams ||
            ((framebuffer == 0 || !texture) && activeParams.Type == RenderTargetType::Texture)) {
            // clear existing resources
            if (texture) {
                texture.reset();
            }
            if (framebuffer) {
                glDeleteFramebuffers(1, &framebuffer);
                framebuffer = 0;
            }

            if (inParams.Type == RenderTargetType::Texture) {
                glGenFramebuffers(1, &framebuffer);
                Begin();
                texture = std::make_unique<OZZ::Texture>();
                texture->Reserve(inParams.Size);
                texture->Bind();

                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture->GetId(), 0);
                GLenum drawBuffer = GL_COLOR_ATTACHMENT0;
                glDrawBuffers(1, &drawBuffer);
                if (auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE) {
                    spdlog::error("Failed to create render target {}", status);
                }
                End();
            } else {
                framebuffer = 0;
            }
        }
        return inParams;
    }
} // namespace OZZ
