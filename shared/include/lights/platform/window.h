//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include "lights/input/input_keys.h"
#include "glm/glm.hpp"

namespace OZZ {
    class Window {
    public:
        using OnWindowCloseCallback = std::function<void()>;
        using OnWindowResizedCallback = std::function<void(glm::ivec2)>;
        using OnKeyPressedCallback = std::function<void(EKey, EKeyState)>;

        Window();
        ~Window();

        void PollEvents();
        void MakeContextCurrent();
        void SwapBuffers();

        [[nodiscard]] glm::ivec2 GetSize() const;
        void* GetWindowHandle() const;
    public:
        OnWindowCloseCallback OnWindowClose;
        OnWindowResizedCallback OnWindowResized;
        OnKeyPressedCallback OnKeyPressed;
    private:
        void initWindow();

    private:
        GLFWwindow* window { nullptr };

        inline static bool bGLADInitialized {false};
    };
} // OZZ