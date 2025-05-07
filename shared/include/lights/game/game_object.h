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

        [[nodiscard]] glm::vec3 GetPosition() const { return position; }
        void SetPosition(const glm::vec3& inPosition) { position = inPosition; updateTransform(); onPositionChanged(); }
        [[nodiscard]] glm::vec3 GetScale() const { return scale; }
        void SetScale(const glm::vec3& inScale) { scale = inScale; updateTransform(); onScaleChanged(); }
        [[nodiscard]] glm::quat GetRotation() const { return rotation; }
        void SetRotation(const glm::quat& inRotation) { rotation = inRotation; updateTransform(); onRotationChanged(); }

        [[nodiscard]] glm::mat4 GetWorldTransform() const { return transform; }

        void SetParent(GameObject* inParent) { parent = inParent; onParentChanged(); }
        [[nodiscard]] OzzWorld2D* GetWorld() const { return physicsWorld.get(); }
    protected:
        virtual void onPositionChanged() {};
        virtual void onScaleChanged() {};
        virtual void onRotationChanged() {};
        virtual void onParentChanged() {};

    private:
        void updateTransform();

    protected:
        GameWorld* gameWorld;;
        std::shared_ptr<OzzWorld2D> physicsWorld { nullptr };
        GameObject* parent { nullptr };

    private:
        glm::vec3 position {0.f};
        glm::vec3 scale {1.f};
        glm::quat rotation {};
        glm::mat4 transform {1.f};
    };
} // O {1.f}ZZ