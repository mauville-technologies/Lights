//
// Created by ozzadar on 2024-12-20.
//

#pragma once
#include <cstdint>

// TODO: Revisit render pass
namespace OZZ {
    enum class RenderTargetType {
        Viewport,
        Texture
    };

    class RenderTarget {
    public:
        explicit RenderTarget(RenderTargetType InType = RenderTargetType::Texture);

        virtual ~RenderTarget() = default;

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
