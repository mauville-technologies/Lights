//
// Created by ozzadar on 2025-03-22.
//

#pragma once
#include "lights/rendering/buffer.h"
#include "lights/rendering/material.h"

namespace OZZ::scene {
    class SceneObject {
    public:
        glm::mat4 Transform{1.f}; // Model transformation matrix
        std::shared_ptr<IndexVertexBuffer> Mesh{nullptr};
        std::shared_ptr<Material> Mat{nullptr}; // Shader/texture parameters
    };
}