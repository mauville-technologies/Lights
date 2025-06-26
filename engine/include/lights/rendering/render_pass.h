//
// Created by ozzadar on 2024-12-20.
//

#pragma once
#include <cstdint>

// TODO: Revisit render pass
namespace OZZ {
    enum class RenderTargetType {
        Window,
        Texture
    };

    class RenderPass {
    public:
        explicit RenderPass(RenderTargetType InType);
        virtual ~RenderPass() = default;

        void Begin();
        void End();

        [[nodiscard]] RenderTargetType GetType() const {
            return type;
        }
    private:
        RenderTargetType type;
        uint32_t framebuffer;
    };
} // OZZ
