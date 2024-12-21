//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <utility>
#include <vector>
#include "game/rendering/buffer.h"
#include "game/rendering/material.h"
#include "lights/input/input_subsystem.h"


namespace OZZ {
    class Camera {
    public:
        glm::mat4 ViewMatrix{1.f};
        glm::mat4 ProjectionMatrix{1.f};
    };

    class SceneObject {
    public:
        glm::mat4 Transform{1.f}; // Model transformation matrix
        std::shared_ptr<IndexVertexBuffer> Mesh{nullptr};
        std::shared_ptr<Material> Mat{nullptr}; // Shader/texture parameters
    };

    class LightSource {
    public:
        glm::vec3 Position;
        glm::vec3 Color;
        float Intensity;
    };

    class SceneLayer {
    public:
        virtual ~SceneLayer() = default;

        virtual void Init(std::shared_ptr<InputSubsystem> Input) {
            input = std::move(Input);
        };
        virtual void Tick(float DeltaTime) {};
        virtual void RenderTargetResized(glm::ivec2 size) = 0;


        Camera LayerCamera {};
        std::vector<std::shared_ptr<SceneObject>> Objects;
        std::vector<std::shared_ptr<LightSource>> Lights;

    protected:
        std::shared_ptr<InputSubsystem> input { nullptr };
    };

    class Scene {
    public:
        virtual void Init(std::shared_ptr<InputSubsystem> Input) {
            input = std::move(Input);
        };

        virtual void Tick(float DeltaTime) {
            for (auto &Layer: Layers) {
                Layer->Tick(DeltaTime);
            }
        };

        // Marked virtual to allow derived Scenes with custom entities
        void RenderTargetResized(glm::ivec2 size) {
            for (auto &Layer: Layers) {
                Layer->RenderTargetResized(size);
            }
        }

        virtual ~Scene() = default;

    public:
        std::vector<std::shared_ptr<SceneLayer>> Layers {};

    protected:
        std::shared_ptr<InputSubsystem> input { nullptr };
    };
}