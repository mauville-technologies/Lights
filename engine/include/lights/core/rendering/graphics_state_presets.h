//
// Created by paulm on 2026-07-01.
//

#pragma once

#include "lights/core/rendering/vertex.h"

namespace OZZ {
    /// Standard alpha-blended graphics state for 2D rendering with the full OZZ::Vertex
    /// attribute layout. Call sites that need a small deviation (topology, blend, depth)
    /// should take the returned struct and override the relevant fields before submitting.
    inline OZZ::rendering::GraphicsStateDescriptor MakeDefault2DGraphicsState() {
        const auto attributeDescriptions = Vertex::GetAttributeDescriptions();
        return {
            .Rasterization =
                {
                    .Cull = OZZ::rendering::CullMode::None,
                    .Front = OZZ::rendering::FrontFace::CounterClockwise,
                },
            .ColorBlend = {OZZ::rendering::ColorBlendAttachmentState {
                .BlendEnable = true,
                .SrcColorFactor = OZZ::rendering::BlendFactor::SrcAlpha,
                .DstColorFactor = OZZ::rendering::BlendFactor::OneMinusSrcAlpha,
                .ColorBlendOp = OZZ::rendering::BlendOp::Add,
                .SrcAlphaFactor = OZZ::rendering::BlendFactor::One,
                .DstAlphaFactor = OZZ::rendering::BlendFactor::OneMinusSrcAlpha,
                .AlphaBlendOp = OZZ::rendering::BlendOp::Add,
                .ColorWriteMask = static_cast<OZZ::rendering::ColorComponentFlags>(
                    OZZ::rendering::ColorComponent::R | OZZ::rendering::ColorComponent::G |
                    OZZ::rendering::ColorComponent::B | OZZ::rendering::ColorComponent::A),
            }},
            .ColorBlendAttachmentCount = 1,
            .VertexInput =
                {
                    .Bindings = {Vertex::GetBindingDescription()},
                    .BindingCount = 1,
                    .Attributes = {attributeDescriptions[0],
                                   attributeDescriptions[1],
                                   attributeDescriptions[2],
                                   attributeDescriptions[3]},
                    .AttributeCount = static_cast<uint32_t>(attributeDescriptions.size()),
                },
        };
    }
} // namespace OZZ
