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
    class GameWorld;

    class GameObject {
    public:
        explicit GameObject(GameWorld* inWorld, std::shared_ptr<OzzWorld2D>);
        virtual ~GameObject() = default;
        virtual void Tick(float DeltaTime) = 0;
        virtual std::vector<scene::SceneObject> GetSceneObjects() = 0;

        [[nodiscard]] glm::vec3 GetPosition() const { return Position; }
        void SetPosition(const glm::vec3& inPosition) { Position = inPosition; onPositionChanged(); }
        [[nodiscard]] glm::vec3 GetScale() const { return Scale; }
        void SetScale(const glm::vec3& inScale) { Scale = inScale; onScaleChanged(); }
        [[nodiscard]] glm::quat GetRotation() const { return Rotation; }
        void SetRotation(const glm::quat& inRotation) { Rotation = inRotation; onRotationChanged(); }

        void SetParent(GameObject* inParent) { parent = inParent; }
        [[nodiscard]] OzzWorld2D* GetWorld() const { return physicsWorld.get(); }
    protected:
        virtual void onPositionChanged() {};
        virtual void onScaleChanged() {};
        virtual void onRotationChanged() {};
        virtual void onParentChanged() {};

    protected:
        glm::vec3 Position {0.f};
        glm::vec3 Scale {1.f};
        glm::quat Rotation {};

        GameWorld* gameWorld;;
        std::shared_ptr<OzzWorld2D> physicsWorld { nullptr };
        GameObject* parent { nullptr };
    };
} // OZZ