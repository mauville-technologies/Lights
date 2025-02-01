//
// Created by ozzadar on 2024-12-18.
//

#include "lights/platform/window.h"
#include "lights/input/glfw_keys.h"
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

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,  4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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

        glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
            if (win->OnKeyPressed) {
                if (action == GLFW_REPEAT) return; // Repeat is not needed.
                GLFWKeyState glfwKeyState(action);
                win->OnKeyPressed(GLFWKey(key), glfwKeyState);
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
        glfwSwapInterval(0);
    }

    void Window::SwapBuffers() {
#ifdef __APPLE__
        static bool firstSwap = false;
        // There's a bug on mac where for some reason the window needs to be moved
        // or resized in order to properly draw the scene. Doing this here
        // will fix the issue on first load on mac. It's ugly but it works.
        if (!firstSwap)
        {
            firstSwap = true;
            auto size = GetSize();
            glfwSetWindowSize(window, size.x+1, size.y+1);
            glfwSetWindowSize(window, size.x, size.y);
        }
#endif
        glfwSwapBuffers(window);
    }

    glm::ivec2 Window::GetSize() const {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return {width, height};
    }

    void *Window::GetWindowHandle() const {
        return window;
    }
} // OZZ