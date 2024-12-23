//
// Created by ozzadar on 2024-12-22.
//

#pragma once
#include <vector>
#include <memory>
#include <imgui.h>
#include <functional>


namespace OZZ {
    class UIComponent {
        friend class UserInterface;
    public:
        virtual ~UIComponent() = default;

    private:
        virtual void registerInDockspace() = 0;
        virtual void render() = 0;

    };

    class UserInterface {
    public:
        void Init(void *windowHandle);
        void Render();
        void Shutdown();

        inline void AddComponent(std::shared_ptr<UIComponent>& Component) {
            components.emplace_back(Component);
        }

        inline void RemoveComponent(std::shared_ptr<UIComponent>& Component) {
            components.erase(std::remove_if(components.begin(), components.end(), [Component](const std::weak_ptr<UIComponent>& c) {
                return c.lock().get() == Component.get();
            }), components.end());
        }

    private:
        void initImGuiDockSpace();
        void finishImGuiDockSpace();
    private:
        std::vector<std::weak_ptr<UIComponent>> components;
    };
} // OZZ