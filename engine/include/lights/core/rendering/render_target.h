//
// Created by ozzadar on 2024-12-20.
//

#pragma once
#include "ozz_rendering/rhi_device.h"
#include "texture.h"

#include <cstdint>
#include <memory>

// TODO: Revisit render pass
namespace OZZ {
    enum class RenderTargetType { Viewport, Texture };

    struct RenderTargetParams {
        RenderTargetType Type{RenderTargetType::Texture};
        glm::uvec2 Size{1, 1};
        glm::vec4 ClearColor{0.f, 0.f, 0.f, 0.f};
        bool bHasDepth{true};

        bool operator==(const RenderTargetParams& other) const { return Type == other.Type && Size == other.Size; }
    };

    class RenderTarget {
    public:
        explicit RenderTarget(rendering::RHIDevice* inDevice, RenderTargetParams&& inParams);
        virtual ~RenderTarget();

        void Begin(rendering::RHIFrameContext& frameContext);

        void End(rendering::RHIFrameContext& frameContext) const;

        [[nodiscard]] RenderTargetType GetType() const { return activeParams.Type; }

        [[nodiscard]] glm::ivec2 GetSize() const { return activeParams.Size; }

        std::shared_ptr<Texture> GetTexture() const { return texture; }

        void Resize(glm::uvec2 inSize);

    private:
        RenderTargetParams setupRenderTarget(const RenderTargetParams& inParams);

    private:
        rendering::RHIDevice* device;
        std::shared_ptr<Texture> texture;
        std::shared_ptr<Texture> depthTexture;
        rendering::RenderPassDescriptor renderPassDescriptor;
        RenderTargetParams activeParams;
    };
} // namespace OZZ
