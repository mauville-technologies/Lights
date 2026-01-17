//
// Created by ozzadar on 2025-06-18.
//

#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <lights/input/input_keys.h>
#include <string>

#if OZZ_PLATFORM_WINDOWS
#undef CreateWindow
#endif

namespace OZZ::platform {
    using OnWindowCloseCallback = std::function<void()>;
    using OnWindowResizedCallback = std::function<void(glm::ivec2)>;
    using OnInputPressedCallback = std::function<void(InputKey, EKeyState)>;
    using OnTextEventCallback = std::function<void(unsigned int)>;
    using OnControllerConnectedCallback = std::function<void(int)>;
    using OnControllerDisconnectedCallback = std::function<void(int)>;
    using OnMouseMoveEventCallback = std::function<void(glm::vec2)>;
    using OnMouseScrollEventCallback = std::function<void(glm::vec2)>;

    struct WindowCallbacks {
        OnWindowCloseCallback OnWindowClose;
        OnWindowResizedCallback OnWindowResized;
        OnInputPressedCallback OnKeyPressed;
        OnTextEventCallback OnTextEvent;
        OnControllerConnectedCallback OnControllerConnected;
        OnControllerDisconnectedCallback OnControllerDisconnected;
        OnMouseMoveEventCallback OnMouseMove;
        OnMouseScrollEventCallback OnMouseScroll;
    };

    class IPlatformWindow {
    public:
        virtual ~IPlatformWindow() = default;
        virtual void CreateWindow(const std::string& title, int width, int height) = 0;
        virtual void* GetProcAddress() = 0;
        virtual void InitInput(WindowCallbacks&& callbacks) = 0;
        virtual void Poll() = 0;
        virtual void MakeContextCurrent() = 0;
        virtual void Present() = 0;
        [[nodiscard]] virtual glm::ivec2 GetSize() const = 0;
        virtual void SetSize(int width, int height) = 0;
        virtual void SetFullscreen(bool fullscreen) = 0;
        virtual void SetTextMode(bool bIsTextMode) = 0;
        [[nodiscard]] virtual bool IsValid() const = 0;

        [[nodiscard]] const auto& GetKeyStates() const { return keyStates; }

        [[nodiscard]] const auto& GetControllerState() const { return controllerState; }

        [[nodiscard]] const auto& GetMouseButtonStates() const { return mouseButtonStates; }

    protected:
        KeyStateArrayType keyStates{};
        MouseButtonStateArrayType mouseButtonStates{};
        ControllerStateMap controllerState;
    };
} // namespace OZZ::platform