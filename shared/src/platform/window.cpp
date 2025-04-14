//
// Created by ozzadar on 2024-12-18.
//

#include "lights/platform/window.h"
#include "lights/input/glfw_keys.h"
#include <spdlog/spdlog.h>

namespace OZZ {
    Window::Window() {
        initWindow();
        initControllers();
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
                if (action == GLFW_REPEAT) action = GLFW_PRESS; // Repeat is not needed.
                GLFWKeyState glfwKeyState(action);
                win->OnKeyPressed({-1, GLFWKey(key)}, glfwKeyState);
            }
        });

        glfwSetJoystickCallback([](int jid, int event) {
            auto win = static_cast<Window*>(glfwGetWindowUserPointer(glfwGetCurrentContext()));
            if (event == GLFW_CONNECTED && glfwJoystickIsGamepad(jid)) {
                win->addController(jid);
            } else if (event == GLFW_DISCONNECTED) {
                win->removeController(jid);
            }
        });
        glfwSetCharCallback(window, [](GLFWwindow *window, unsigned int codepoint) {
            auto win = static_cast<Window*>(glfwGetWindowUserPointer(glfwGetCurrentContext()));
            if (win->OnTextEvent) {
                win->OnTextEvent(codepoint);
            }
        });
    }

    void Window::initControllers() {
        // On startup, we need to detect connected controllers
        for (int i = 0; i < GLFW_JOYSTICK_LAST; ++i) {
            if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i)) {
                addController(i);
            }
        }
    }

    void Window::addController(int index) {
        spdlog::info("Controller {} connected", index);
        controllerState[index] = {};
        if (OnControllerConnected) {
            OnControllerConnected(index);
        }
    }

    void Window::removeController(int index) {
        const auto controllerRemoved = controllerState.erase(index);
        if (controllerRemoved) {
            spdlog::info("Controller {} disconnected", index);
            if (OnControllerDisconnected) {
                OnControllerDisconnected(index);
            }
        }
    }

    void Window::PollEvents() {
        if (glfwWindowShouldClose(window)) {
            if (OnWindowClose) {
                OnWindowClose();
            }
        }
        glfwPollEvents();

        // Get all key states from glfw
        for (int i = 0; i < GLFW_KEY_LAST; ++i) {
            const auto keyIndex = static_cast<int>(GLFWKey(i));
            if (keyIndex == +EKey::KeyCount) continue;
            keyStates[keyIndex] = GLFWKeyState(glfwGetKey(window, i));
        }

        // get all controller states
        for (auto &[index, controller]: controllerState) {
            if (glfwJoystickIsGamepad(index)) {
                GLFWgamepadstate gamepadState;
                if (glfwGetGamepadState(index, &gamepadState)) {
                    // update all buttons
                    for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; ++i) {
                        const auto newState = gamepadState.buttons[i];
                        if (newState != controller[i]) {
                            controller[i] = newState;
                            if (OnKeyPressed) {
                                OnKeyPressed({index, static_cast<EControllerButton>(i)}, GLFWKeyState(newState));
                            }
                        }
                    }

                    // update all axes
                    for (int i = 0; i < GLFW_GAMEPAD_AXIS_LAST; ++i) {
                        controller[i + +EControllerButton::LeftStickX] = gamepadState.axes[i];
                    }
                }
            }
        }

        //TODO: Add mouse and joystick input
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