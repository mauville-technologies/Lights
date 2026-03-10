//
// Created by ozzadar on 2025-03-22.
//

#pragma once
#include "lights/core/rendering/buffer.h"
#include "lights/core/rendering/material.h"

namespace OZZ::scene {
    struct Mesh {
        rendering::RHIBufferHandle VertexBuffer{rendering::RHIBufferHandle::Null()};
        rendering::RHIBufferHandle IndexBuffer{rendering::RHIBufferHandle::Null()};
        uint64_t VertexCount{0};
        uint64_t IndexCount{0};
    };

    struct SceneObject {
        glm::mat4 Transform{1.f};               // Model transformation matrix
        Mesh MeshData{};                        // Geometry data (vertex/index buffers)
        std::shared_ptr<Material> Mat{nullptr}; // Shader/texture parameters
    };
} // namespace OZZ::scene