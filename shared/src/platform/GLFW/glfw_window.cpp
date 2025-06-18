//
// Created by ozzadar on 2025-06-18.
//

#include "glfw_window.h"

#ifdef OZZ_GLFW
#include <spdlog/spdlog.h>
#include "glfw_keys.h"

namespace OZZ::platform::glfw {
    GLFWWindow::~GLFWWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void GLFWWindow::CreateWindow(const std::string &title, int width, int height) {
        if (!glfwInit()) {
            spdlog::error("Failed to initialize GLFW");
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(800, 600, "Ozzadar", nullptr, nullptr);
        if (!window) {
            spdlog::error("Failed to create window");
            return;
        }
    }

    void *GLFWWindow::GetProcAddress() {
        return reinterpret_cast<void *>(glfwGetProcAddress);
    }

    void GLFWWindow::InitInput(WindowCallbacks &&inCallbacks) {
        callbacks = std::move(inCallbacks);
        glfwSetWindowUserPointer(window, this);

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
            if (const auto win = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(window)); win->callbacks.
                OnWindowResized) {
                win->callbacks.OnWindowResized({width, height});
            }
        });

        glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            if (const auto win = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(window)); win->callbacks.
                OnKeyPressed) {
                const GLFWKeyState glfwKeyState(action);
                win->callbacks.OnKeyPressed({EDeviceID::Keyboard, GLFWKey(key)}, glfwKeyState);
            }
        });

        glfwSetJoystickCallback([](int jid, int event) {
            const auto win = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(glfwGetCurrentContext()));
            if (event == GLFW_CONNECTED && glfwJoystickIsGamepad(jid)) {
                win->addController(jid);
            } else if (event == GLFW_DISCONNECTED) {
                win->removeController(jid);
            }
        });
        glfwSetCharCallback(window, [](GLFWwindow *window, unsigned int codepoint) {
            if (const auto win = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(glfwGetCurrentContext())); win->
                callbacks.OnTextEvent) {
                win->callbacks.OnTextEvent(codepoint);
            }
        });
        glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
            if (const auto win = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(glfwGetCurrentContext())); win->
                callbacks.OnKeyPressed) {
                win->callbacks.OnKeyPressed({EDeviceID::Mouse, static_cast<EMouseButton>(button)},
                                            GLFWKeyState(action));
            }
        });
        glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
            if (const auto win = static_cast<GLFWWindow *>(glfwGetWindowUserPointer(glfwGetCurrentContext())); win->
                callbacks.OnMouseMove) {
                win->callbacks.OnMouseMove({static_cast<float>(xpos), static_cast<float>(ypos)});
            }
        });

        // On startup, we need to detect connected controllers
        for (int i = 0; i < GLFW_JOYSTICK_LAST; ++i) {
            if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i)) {
                addController(i);
            }
        }
    }

    void GLFWWindow::Poll() {
        if (glfwWindowShouldClose(window)) {
            if (callbacks.OnWindowClose) {
                callbacks.OnWindowClose();
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
            if (glfwJoystickIsGamepad(+index)) {
                GLFWgamepadstate gamepadState;
                if (glfwGetGamepadState(+index, &gamepadState)) {
                    // update all buttons
                    for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; ++i) {
                        const auto newState = gamepadState.buttons[i];
                        if (newState != controller[i]) {
                            controller[i] = newState;
                            if (callbacks.OnKeyPressed) {
                                callbacks.OnKeyPressed({index, static_cast<EControllerButton>(i)},
                                                       GLFWKeyState(newState));
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
    }

    void GLFWWindow::MakeContextCurrent() {
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);
    }

    void GLFWWindow::Present() {
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

    glm::ivec2 GLFWWindow::GetSize() const {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return {width, height};
    }

    void GLFWWindow::SetSize(int width, int height) {
        if (!glfwGetWindowMonitor(window)) {
            // Only set size if we're in windowed mode
            glfwSetWindowSize(window, width, height);
            // Center the window on the screen
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            int xpos = (mode->width - width) / 2;
            int ypos = (mode->height - height) / 2;
            glfwSetWindowPos(window, xpos, ypos);
        }
    }

    void GLFWWindow::SetFullscreen(bool fullscreen) {
        if (fullscreen) {
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);

            // Store the current window position and size before going fullscreen
            int xpos, ypos, width, height;
            glfwGetWindowPos(window, &xpos, &ypos);
            glfwGetWindowSize(window, &width, &height);

            // Remove window decorations
            glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);

            // Set the window to cover the entire monitor
            glfwSetWindowPos(window, 0, 0);
            glfwSetWindowSize(window, mode->width, mode->height);
        } else {
            // Restore window decorations
            glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);

            // Restore to windowed mode with the last known windowed size
            glfwSetWindowMonitor(window, nullptr, 100, 100, 800, 600, 0);
        }
    }

    void GLFWWindow::addController(int index) {
        spdlog::info("Controller {} connected", index);
        controllerState[static_cast<EDeviceID>(index)] = {};
        if (callbacks.OnControllerConnected) {
            callbacks.OnControllerConnected(index);
        }
    }

    void GLFWWindow::removeController(int index) {
        if (controllerState.erase(static_cast<EDeviceID>(index))) {
            spdlog::info("Controller {} disconnected", index);
            if (callbacks.OnControllerDisconnected) {
                callbacks.OnControllerDisconnected(index);
            }
        }
    }
} // OZZ
#endif
