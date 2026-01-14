//
// Created by ozzadar on 2024-12-31.
//

#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <lights/scene/scene_object.h>
#include <ozz_collision/world.h>

namespace OZZ {
    class GameWorld;

    class GameObject {
    public:
        explicit GameObject(uint64_t inId, GameWorld* inWorld);
        virtual ~GameObject() = default;
        virtual void Tick(float DeltaTime) = 0;
        virtual std::vector<scene::SceneObject> GetSceneObjects() = 0;

        [[nodiscard]] glm::vec3 GetPosition() const { return position; }

        void SetPosition(const glm::vec3& inPosition) {
            position = inPosition;
            updateTransform();
            onPositionChanged();
        }

        [[nodiscard]] glm::vec3 GetScale() const { return scale; }

        void SetScale(const glm::vec3& inScale) {
            scale = inScale;
            updateTransform();
            onScaleChanged();
        }

        [[nodiscard]] glm::quat GetRotation() const { return rotation; }

        void SetRotation(const glm::quat& inRotation) {
            rotation = inRotation;
            updateTransform();
            onRotationChanged();
        }

        [[nodiscard]] glm::mat4 GetWorldTransform() const;

        [[nodiscard]] glm::vec3 GetWorldPosition() const { return glm::vec3(transform[3]); }

        [[nodiscard]] glm::vec3 GetWorldScale() const {
            return glm::vec3(glm::length(glm::vec3(transform[0])),
                             glm::length(glm::vec3(transform[1])),
                             glm::length(glm::vec3(transform[2])));
        }

        [[nodiscard]] glm::quat GetWorldRotation() const {
            glm::vec3 scale = GetWorldScale();
            glm::mat3 rotMat(glm::vec3(transform[0]) / scale.x,
                             glm::vec3(transform[1]) / scale.y,
                             glm::vec3(transform[2]) / scale.z);
            return glm::quat_cast(rotMat);
        }

        void SetParent(GameObject* inParent) {
            parent = inParent;
            onParentChanged();
        }

    protected:
        virtual void onPositionChanged() {};
        virtual void onScaleChanged() {};
        virtual void onRotationChanged() {};
        virtual void onParentChanged() {};

    private:
        virtual void updateTransform();

    protected:
        GameWorld* gameWorld;
        GameObject* parent{nullptr};

        uint64_t id{0};

    private:
        glm::vec3 position{0.f};
        glm::vec3 scale{1.f};
        glm::quat rotation{};
        glm::mat4 transform{1.f};
    };
} // namespace OZZ