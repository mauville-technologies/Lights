//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <unordered_map>
#include <functional>
#include "lights/input/input_keys.h"
#include "glm/glm.hpp"
#include <variant>

namespace OZZ {
    class GLFWKey;

    class Window {
    public:
        using OnWindowCloseCallback = std::function<void()>;
        using OnWindowResizedCallback = std::function<void(glm::ivec2)>;
        using OnInputPressedCallback = std::function<void(InputKey, EKeyState)>;
        using OnTextEventCallback = std::function<void(unsigned int)>;
        using OnControllerConnectedCallback = std::function<void(int)>;
        using OnControllerDisconnectedCallback = std::function<void(int)>;

        Window();
        ~Window();

        void PollEvents();
        void MakeContextCurrent();
        void SwapBuffers();

        [[nodiscard]] glm::ivec2 GetSize() const;
        [[nodiscard]] void* GetWindowHandle() const;
        [[nodiscard]] const auto& GetKeyStates() const { return keyStates; }
        [[nodiscard]] const auto& GetControllerState() const { return controllerState; }

    private:
        void initWindow();
        void initControllers();

        void addController(int index);
        void removeController(int index);
    public:
        OnWindowCloseCallback OnWindowClose;
        OnWindowResizedCallback OnWindowResized;
        OnInputPressedCallback OnKeyPressed;
        OnTextEventCallback OnTextEvent;
        OnControllerConnectedCallback OnControllerConnected;
        OnControllerDisconnectedCallback OnControllerDisconnected;

    private:
        GLFWwindow* window { nullptr };

        inline static bool bGLADInitialized {false};
        KeyStateArrayType keyStates;
        ControllerStateMap controllerState;
    };
} // OZZ