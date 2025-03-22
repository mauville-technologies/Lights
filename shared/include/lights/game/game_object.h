//
// Created by ozzadar on 2024-12-31.
//

#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <ozz_collision/world.h>
#include <lights/scene/scene_object.h>

namespace OZZ {
    class GameObject {
    public:
        explicit GameObject(std::shared_ptr<OzzWorld2D>);
        virtual ~GameObject() = default;
        virtual void Tick(float DeltaTime) = 0;
        virtual std::vector<scene::SceneObject> GetSceneObjects() = 0;

        [[nodiscard]] glm::vec3 GetPosition() const { return Position; }
        [[nodiscard]] glm::vec3 GetScale() const { return Scale; }
        [[nodiscard]] glm::quat GetRotation() const { return Rotation; }

        [[nodiscard]] OzzWorld2D* GetWorld() const { return world.get(); }
    protected:
        glm::vec3 Position;
        glm::vec3 Scale;
        glm::quat Rotation;

        std::shared_ptr<OzzWorld2D> world;
    };
} // OZZ