//
// Created by ozzadar on 2024-12-20.
//

#include "render_pass.h"
#include <glad/glad.h>

namespace OZZ {
    RenderPass::RenderPass(RenderTargetType InType) : type(InType) {
        if (type == RenderTargetType::Texture) {
            glGenFramebuffers(1, &framebuffer);
        } else {
            framebuffer = 0;
        }
    }

    void RenderPass::Begin() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    void RenderPass::End() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // OZZ