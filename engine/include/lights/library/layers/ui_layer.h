//
// Created by ozzadar on 2025-05-22.
//

#pragma once
#include <lights/game/game_world.h>
#include <lights/scene/scene_layer.h>
#include <unordered_map>
#include <any>

#include "lights/library/objects/ui/ui_component.h"

namespace OZZ {
    class InputSubsystem;
}

namespace OZZ::lights::library::layers {
    class UILayer: public OZZ::scene::SceneLayer {
    public:

        explicit UILayer(GameWorld* inGameWorld) : SceneLayer(), gameWorld(inGameWorld) {};
        ~UILayer() override = default;

        void SetInputSubsystem(const std::shared_ptr<OZZ::InputSubsystem>& inInput);

        // Override the Init function to initialize the components
        void Init() override;
        void PhysicsTick(float DeltaTime) override;
        void Tick(float DeltaTime) override;
        void RenderTargetResized(glm::ivec2 size) override;
        std::vector<scene::SceneObject> GetSceneObjects() override;

        template <typename ComponentType>
        requires IsUIComponent<ComponentType>
        OZZ::GameObjectContainer<ComponentType> AddComponent(const std::string& name, const typename ComponentType::ParamsType& inParams, bool bCanFocus = false) {
            RemoveComponent(name);

            auto newObject = gameWorld->CreateGameObject<ComponentType>(inParams);
            components[name] = {newObject.first, static_cast<game::objects::UIComponent*>(newObject.second)};

            if (bCanFocus) {
                if (std::ranges::find(focusOrder, name) == focusOrder.end()) {
                    focusOrder.push_back(name);
                }
            }

            return newObject;
        }

        void RemoveComponent(const std::string& name);

        template <typename ComponentType>
        requires IsUIComponent<ComponentType>
        OZZ::GameObjectContainer<ComponentType> GetComponent(const std::string& name) {
            if (components.contains(name)) {
                const auto [id, ptr] = components[name];
                return {id, static_cast<ComponentType*>(ptr)};
            }

            spdlog::warn("Component with name {} does not exist.", name);
            return nullptr;
        }

        void SetFocusOrder( const std::vector<std::string>& inFocusOrder);
        void CycleFocus(bool bForward = true);

        void Clear();
    private:
        void setFocus(const std::string& name);
        void setFocus(size_t index);
        void removeFromFocusOrder(const std::string& name);
    private:
        GameWorld* gameWorld { nullptr };
        std::unordered_map<std::string, OZZ::GameObjectContainer<game::objects::UIComponent>> components;

        uint32_t currentFocusIndex { 0 };
        std::vector<std::string> focusOrder {};

        int width { 1920 };
        int height { 1080 };
        bool bShiftPressed { false };
        std::shared_ptr<OZZ::InputSubsystem> input { nullptr };
    };
} // OZZ