//
// Created by ozzadar on 2024-12-18.
//

#include "lights/platform/window.h"
#include <spdlog/spdlog.h>

namespace OZZ {
    Window::Window() {
        initWindow();
    }

    Window::~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::initWindow() {
        if (!glfwInit()) {
            spdlog::error("Failed to initialize GLFW");
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(800, 600, "Ozzadar", nullptr, nullptr);
        if (!window) {
            spdlog::error("Failed to create window");
            return;
        }

        MakeContextCurrent();
        if (!bGLADInitialized) {
            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                spdlog::error("Failed to initialize GLAD");
                return;
            }
            bGLADInitialized = true;
        }

        glfwSetWindowUserPointer(window, this);

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
            auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
            if (win->OnWindowResized) {
                win->OnWindowResized({width, height});
            }
        });
    }

    void Window::PollEvents() {
        if (glfwWindowShouldClose(window)) {
            if (OnWindowClose) {
                OnWindowClose();
            }
        }
        glfwPollEvents();
    }

    void Window::MakeContextCurrent() {
        glfwMakeContextCurrent(window);
    }

    void Window::SwapBuffers() {
        glfwSwapBuffers(window);
    }

    glm::ivec2 Window::GetSize() const {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return {width, height};
    }
} // OZZ