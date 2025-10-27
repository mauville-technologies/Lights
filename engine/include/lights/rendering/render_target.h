//
// Created by ozzadar on 2024-12-20.
//

#pragma once
#include "texture.h"

#include <cstdint>
#include <memory>

// TODO: Revisit render pass
namespace OZZ {
    enum class RenderTargetType { Viewport, Texture };

    struct RenderTargetParams {
        RenderTargetType Type{RenderTargetType::Texture};
        glm::ivec2 Size{1, 1};

        bool operator==(const RenderTargetParams& other) const { return Type == other.Type && Size == other.Size; }
    };

    class RenderTarget {
    public:
        explicit RenderTarget(RenderTargetParams&& inParams);
        virtual ~RenderTarget() = default;

        void Begin();

        void End();

        [[nodiscard]] RenderTargetType GetType() const { return activeParams.Type; }

        [[nodiscard]] glm::ivec2 GetSize() const { return activeParams.Size; }

        std::shared_ptr<Texture> GetTexture() const { return texture; }

        void Resize(glm::ivec2 inSize);

    private:
        RenderTargetParams setUpRenderTarget(const RenderTargetParams& inParams);

    private:
        RenderTargetParams activeParams;
        std::shared_ptr<Texture> texture;
        uint32_t framebuffer{0};
    };
} // namespace OZZ
