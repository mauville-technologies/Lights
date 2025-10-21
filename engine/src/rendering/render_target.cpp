//
// Created by ozzadar on 2024-12-20.
//

#include "lights/rendering/render_target.h"
#include <glad/glad.h>

namespace OZZ {
    RenderTarget::RenderTarget(RenderTargetType InType) : type(InType) {
        if (type == RenderTargetType::Texture) {
            glGenFramebuffers(1, &framebuffer);
        } else {
            framebuffer = 0;
        }
    }

    void RenderTarget::Begin() {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    void RenderTarget::End() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // OZZ
