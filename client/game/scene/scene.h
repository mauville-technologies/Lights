//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include "game/rendering/buffer.h"
#include "game/rendering/material.h"


namespace OZZ {
class Camera {
public:
    glm::mat4 ViewMatrix { 1.f };
    glm::mat4 ProjectionMatrix { 1.f };
};

class SceneObject {
public:
    glm::mat4 Transform { 1.f }; // Model transformation matrix
    std::shared_ptr<IndexVertexBuffer> Mesh { nullptr };
    std::shared_ptr<Material> Mat { nullptr }; // Shader/texture parameters
};

class LightSource {
public:
    glm::vec3 Position;
    glm::vec3 Color;
    float Intensity;
};

class Scene {
public:
    virtual void Init() = 0;
    virtual void Tick(float DeltaTime) = 0;

    // Marked virtual to allow derived Scenes with custom entities
    virtual const std::vector<std::shared_ptr<SceneObject>>& GetObjects() const = 0;
    virtual const std::vector<std::shared_ptr<LightSource>>& GetLights() const = 0;
    virtual std::shared_ptr<Camera> GetCamera() const = 0;

    virtual void RenderTargetResized(glm::ivec2 size) = 0;

    virtual ~Scene() = default;
};
}