//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <functional>
#include "glm/glm.hpp"

namespace OZZ {
    class Window {
    public:
        using OnWindowCloseCallback = std::function<void()>;
        using OnWindowResizedCallback = std::function<void(glm::ivec2)>;

        Window();
        ~Window();

        void PollEvents();
        void MakeContextCurrent();
        void SwapBuffers();

        [[nodiscard]] glm::ivec2 GetSize() const;
    public:
        OnWindowCloseCallback OnWindowClose;
        OnWindowResizedCallback OnWindowResized;
    private:
        void initWindow();

    private:
        GLFWwindow* window { nullptr };

        inline static bool bGLADInitialized {false};
    };
} // OZZ