//
// Created by ozzadar on 2024-12-19.
//

#pragma once

#include "buffer.h"
#include "shader.h"
#include "texture.h"

#include <memory>
#include <string>
#include <utility>
#include <variant>

namespace OZZ {
    enum class DrawMode { Triangles, Lines, LineLoop, Points };

    inline rendering::PrimitiveTopology ToRHIPrimitiveTopology(const DrawMode& mode) {
        switch (mode) {
            case DrawMode::Points:
                return rendering::PrimitiveTopology::PointList;
            case DrawMode::Lines:
                return rendering::PrimitiveTopology::LineList;
            case DrawMode::LineLoop:
                // Line loop isn't directly supported in modern APIs, so we might need to emulate it with an index
                // buffer
                return rendering::PrimitiveTopology::LineList;
            case DrawMode::Triangles:
            default:
                return rendering::PrimitiveTopology::TriangleList;
        }
    }

    class Material {
    public:
        struct MaterialSettings {
            DrawMode Mode{DrawMode::Triangles};
            float LineWidth{1.f};
            float PointSize{1.f};
            bool bHasScissor{false};
            glm::ivec4 Scissor{0, 0, 0, 0};
        };

        Material(rendering::RHIDevice* inDevice)
            : device(inDevice) {}

        ~Material();

        void Bind(rendering::RHIFrameContext& frameContext);

        void SetShader(std::shared_ptr<Shader> inShader) { this->shader = std::move(inShader); }

        [[nodiscard]] std::shared_ptr<Shader> GetShader() const { return shader; }

        bool SetResource(uint32_t set,
                         uint32_t binding,
                         const std::variant<rendering::RHITextureHandle, rendering::RHIBufferHandle>& resource);

        void RemoveResource(uint32_t set, uint32_t binding);

        MaterialSettings& GetSettings() { return settings; }

        [[nodiscard]] const MaterialSettings& GetSettings() const { return settings; }

    private:
        rendering::RHIDevice* device;
        std::unordered_map<uint32_t, rendering::RHIDescriptorSetHandle> descriptorSets;
        std::unordered_map<uint32_t, bool> dirtyDescriptorSets;

        std::shared_ptr<Shader> shader;
        std::unordered_map<
            uint32_t,
            std::unordered_map<uint32_t, std::variant<rendering::RHITextureHandle, rendering::RHIBufferHandle>>>
            resources;
        MaterialSettings settings;
    };
} // namespace OZZ
