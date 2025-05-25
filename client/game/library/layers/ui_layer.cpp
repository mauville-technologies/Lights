//
// Created by ozzadar on 2025-05-22.
//

#include "ui_layer.h"

#include <ranges>
#include <lights/scene/scene.h>

namespace OZZ::lights::library::layers {
    void UILayer::Init() {
        SceneLayer::Init();

        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), // Camera position
                                             glm::vec3(0.f, 0.f, 0.f), // Target to look at
                                             glm::vec3(0.f, 1.f, 0.f)); // Up vector
    }

    void UILayer::PhysicsTick(float DeltaTime) {
        SceneLayer::PhysicsTick(DeltaTime);
    }

    void UILayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);
        // tick all components
        for (const auto &[id, ptr]: components | std::views::values) {
            ptr->Tick(DeltaTime);
        }
    }

    void UILayer::RenderTargetResized(glm::ivec2 size) {
        const int width = size.x;
        const int height = size.y;

        LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.001f,
                                                  1000.f);
    }

    std::vector<OZZ::scene::SceneObject> UILayer::GetSceneObjects() {
        auto sceneObjects = std::vector<scene::SceneObject>{};
        for (const auto &[id, ptr]: components | std::views::values) {
            sceneObjects = sceneObjects + ptr->GetSceneObjects();
        }
        return sceneObjects;
    }

    void UILayer::RemoveComponent(const std::string &name)  {
        if (components.contains(name)) {
            gameWorld->RemoveObject(components[name].first);
            components.erase(name);
            // remove from focus order if it exists
            std::erase(focusOrder, name);
            return;
        }
        spdlog::warn("Component with name {} does not exist.", name);
    }

    void UILayer::SetFocusOrder(const std::vector<std::string> &inFocusOrder) {
        // remove all components from the focus order
        focusOrder.clear();
        for (const auto &name: inFocusOrder) {
            if (components.contains(name)) {
                focusOrder.push_back(name);
            } else {
                spdlog::warn("Component with name {} does not exist.", name);
            }
        }
    }


    void UILayer::setFocus(const std::string& name) {
        // find in focus order
        auto it = std::ranges::find(focusOrder, name);
        if (it != focusOrder.end()) {
            const size_t index = std::distance(focusOrder.begin(), it);
            setFocus(index);

        } else {
            spdlog::warn("Component with name {} does not exist in focus order.", name);
        }
    }

    void UILayer::setFocus(const size_t index) {
        // remove focus from current component
        if (index != currentFocusIndex && currentFocusIndex < focusOrder.size()) {
            const auto &[id, ptr] = components[focusOrder[currentFocusIndex]];
            // TODO: call component focus method
        }

        if (index >= focusOrder.size()) {
            spdlog::warn("Index {} is out of range for focus order.", index);
            if (focusOrder.empty()) {
                spdlog::warn("Focus order is empty.");
                currentFocusIndex = 0;
            }
            return;
        }

        const auto &[id, ptr] = components[focusOrder[index]];
        //TODO: call component focus method
        currentFocusIndex = index;
    }

    void UILayer::removeFromFocusOrder(const std::string& name) {
        auto it = std::find(focusOrder.begin(), focusOrder.end(), name);
        if (it != focusOrder.end()) {
            size_t index = std::distance(focusOrder.begin(), it);
            focusOrder.erase(it);

            // if current focus index is the erased index, reset focus
            setFocus(0);
        }
    }
}