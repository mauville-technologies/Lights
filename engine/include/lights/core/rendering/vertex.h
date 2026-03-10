//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include "ozz_rendering/rhi_pipeline_state.h"

#include <glm/glm.hpp>

namespace OZZ {
    struct Vertex {
        glm::vec3 Position{0.f, 0.f, 0.f};
        glm::vec4 Color{1.f, 1.f, 1.f, 1.f};
        glm::vec3 Normal{0.f, 0.f, 0.f};
        glm::vec2 UV{0.f, 0.f};

        static OZZ::rendering::VertexInputBindingDescriptor GetBindingDescription() {
            return {
                .Binding = 0,
                .Stride = sizeof(Vertex),
                .InputRate = OZZ::rendering::VertexInputRate::Vertex,
            };
        }

        static std::array<OZZ::rendering::VertexInputAttributeDescriptor, 4> GetAttributeDescriptions() {
            return {
                OZZ::rendering::VertexInputAttributeDescriptor{
                    .Location = 0,
                    .Binding = 0,
                    .Format = OZZ::rendering::VertexFormat::Float3,
                    .Offset = offsetof(Vertex, Position),
                },
                OZZ::rendering::VertexInputAttributeDescriptor{
                    .Location = 1,
                    .Binding = 0,
                    .Format = OZZ::rendering::VertexFormat::Float4,
                    .Offset = offsetof(Vertex, Color),
                },
                OZZ::rendering::VertexInputAttributeDescriptor{
                    .Location = 2,
                    .Binding = 0,
                    .Format = OZZ::rendering::VertexFormat::Float3,
                    .Offset = offsetof(Vertex, Normal),
                },
                OZZ::rendering::VertexInputAttributeDescriptor{
                    .Location = 3,
                    .Binding = 0,
                    .Format = OZZ::rendering::VertexFormat::Float2,
                    .Offset = offsetof(Vertex, UV),
                },
            };
        }
    };
} // namespace OZZ
