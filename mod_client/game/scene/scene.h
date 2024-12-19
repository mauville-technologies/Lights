//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <vector>

namespace OZZ {
class Camera {
public:
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
};

class SceneObject {
public:
    glm::mat4 Transform; // Model transformation matrix
//    std::shared_ptr<Mesh> MeshData;
//    std::shared_ptr<Material> MaterialData; // Shader/texture parameters
};

class LightSource {
public:
    glm::vec3 Position;
    glm::vec3 Color;
    float Intensity;
};

class Scene {
public:
    virtual void Tick(float DeltaTime) = 0;

    // Marked virtual to allow derived Scenes with custom entities
    virtual const std::vector<std::shared_ptr<SceneObject>>& GetObjects() const = 0;
    virtual const std::vector<std::shared_ptr<LightSource>>& GetLights() const = 0;
    virtual std::shared_ptr<Camera> GetCamera() const = 0;

    virtual ~Scene() = default;
};
}