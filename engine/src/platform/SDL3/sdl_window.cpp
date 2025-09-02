//
// Created by ozzadar on 2025-06-18.
//

#include "sdl_window.h"

#include "sdl_keys.h"

#ifdef OZZ_SDL3

#include <spdlog/spdlog.h>

namespace OZZ::platform::SDL3 {
    void SDLWindow::CreateWindow(const std::string &title, int width, int height) {
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD) < 0) {
            spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
            return;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        window = SDL_CreateWindow(
            title.c_str(),
            width,
            height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
        if (!window) {
            spdlog::error("Failed to create SDL window: {}", SDL_GetError());
            return;
        }

        glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
            spdlog::error("Failed to create OpenGL context: {}", SDL_GetError());
            SDL_DestroyWindow(window);
            return;
        }

        MakeContextCurrent();
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(window);
    }

    void *SDLWindow::GetProcAddress() {
        return reinterpret_cast<void *>(SDL_GL_GetProcAddress);
    }

    void SDLWindow::InitInput(WindowCallbacks &&inCallbacks) {
        callbacks = std::move(inCallbacks);
    }

    void SDLWindow::Poll() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_GAMEPAD_ADDED: {
                    addGamepad(event.jdevice.which);
                    break;
                }
                case SDL_EVENT_GAMEPAD_REMOVED: {
                    // does it exist in the sdlGamepads list
                    clearGamepad(event.jdevice.which);

                    break;
                }
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                case SDL_EVENT_GAMEPAD_BUTTON_UP: {
                    auto button = sdl3::SDLGamepadButton(static_cast<SDL_GamepadButton>(event.gbutton.button));
                    const sdl3::SDLKeyState newButtonState(static_cast<int>(event.type));
                    // get deviceID from gamepadIDs mapping
                    auto it = std::find(gamepadIDs.begin(), gamepadIDs.end(), event.gbutton.which);
                    if (it == gamepadIDs.end()) {
                        spdlog::error("Gamepad with ID {} not found", event.gbutton.which);
                        continue;
                    }
                    auto deviceID = static_cast<EDeviceID>(std::distance(gamepadIDs.begin(), it));
                    if (auto &controller = controllerState[deviceID]; controller[button] != newButtonState) {
                        controller[button] = newButtonState;
                        if (callbacks.OnKeyPressed) {
                            callbacks.OnKeyPressed({deviceID, button}, newButtonState);
                        }
                    }
                    break;
                }
                case SDL_EVENT_QUIT: {
                    if (callbacks.OnWindowClose) {
                        callbacks.OnWindowClose();
                    }
                    break;
                }
                case SDL_EVENT_WINDOW_RESIZED: {
                    if (callbacks.OnWindowResized) {
                        callbacks.OnWindowResized({event.window.data1, event.window.data2});
                    }
                    break;
                }
                case SDL_EVENT_TEXT_INPUT: {
                    if (callbacks.OnTextEvent) {
                        // Convert UTF-8 to Unicode code point
                        unsigned int unicode = 0;
                        if (event.text.text[0] != '\0') {
                            unicode = static_cast<unsigned int>(event.text.text[0]);
                        }
                        callbacks.OnTextEvent(unicode);
                    }
                    break;
                }
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP: {
                    spdlog::info("Key event: {} {}", event.type, event.key.key);
                    const sdl3::SDLKey sdlKey(static_cast<int>(event.key.key));
                    if (sdlKey == EKey::KeyCount) {
                        spdlog::error("Invalid key code: {}", event.key.key);
                        continue;
                    }
                    const sdl3::SDLKeyState newKeyState(static_cast<int>(event.type));
                    if (auto oldKeyState = keyStates[sdlKey]; oldKeyState != newKeyState) {
                        keyStates[sdlKey] = newKeyState;
                        if (callbacks.OnKeyPressed) {
                            callbacks.OnKeyPressed({EDeviceID::Keyboard, sdlKey}, newKeyState);
                        }
                    }
                    break;
                }
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    const sdl3::SDLMouseButton sdlMouseButton(event.button.button);
                    const sdl3::SDLKeyState newMouseState(static_cast<int>(event.type));
                    if (auto oldMouseState = mouseButtonStates[sdlMouseButton]; oldMouseState != newMouseState) {
                        mouseButtonStates[sdlMouseButton] = newMouseState;
                        if (callbacks.OnKeyPressed) {
                            callbacks.OnKeyPressed({EDeviceID::Mouse, sdlMouseButton}, newMouseState);
                        }
                    }
                    break;
                }
                case SDL_EVENT_MOUSE_MOTION: {
                    if (callbacks.OnMouseMove) {
                        const auto mousePos = glm::vec2(event.motion.x, event.motion.y);
                        callbacks.OnMouseMove(mousePos);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        for (const auto &[deviceID, gamepad] : gamepads) {
            if (gamepad) {
                // Update the controller state
                for (int i = SDL_GAMEPAD_AXIS_LEFTX; i < SDL_GAMEPAD_AXIS_COUNT; ++i) {
                    auto button = sdl3::SDLGamepadButton(static_cast<SDL_GamepadAxis>(i));
                    const auto value = static_cast<float>(SDL_GetGamepadAxis(gamepad, static_cast<SDL_GamepadAxis>(i)));
                    controllerState[deviceID][button] = value / static_cast<float>(SDL_JOYSTICK_AXIS_MAX);
                }
            }
        }
    }

    void SDLWindow::MakeContextCurrent() {
        SDL_GL_MakeCurrent(window, glContext);
        SDL_GL_SetSwapInterval(0);
    }

    void SDLWindow::Present() {
        SDL_GL_SwapWindow(window);
    }

    glm::ivec2 SDLWindow::GetSize() const {
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        return {width, height};
    }

    void SDLWindow::SetSize(int width, int height) {
        SDL_SetWindowSize(window, width, height);
    }

    void SDLWindow::SetFullscreen(const bool fullscreen) {
        SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : false);
    }

    void SDLWindow::SetTextMode(const bool bIsTextMode) {
        if (bIsTextMode) {
            SDL_StartTextInput(window);
        } else {
            SDL_StopTextInput(window);
        }
    }

    void SDLWindow::addGamepad(int sdlIndex) {
        if (auto *newGamepad = SDL_OpenGamepad(sdlIndex)) {
            // Device ID is the next available index
            const auto availableIndexIterator = std::ranges::find(gamepadIDs, -1);
            if (availableIndexIterator == gamepadIDs.end()) {
                spdlog::error("No available gamepad slots");
                return;
            }
            const auto availableIndex =
                static_cast<EDeviceID>(std::distance(gamepadIDs.begin(), availableIndexIterator));
            spdlog::info("Gamepad connected: {}", +availableIndex);
            gamepads[availableIndex] = newGamepad;
            controllerState[availableIndex] = {};
            gamepadIDs[static_cast<size_t>(availableIndex)] = sdlIndex;
            if (callbacks.OnControllerConnected) {
                callbacks.OnControllerConnected(+availableIndex);
            }
        }
    }

    void SDLWindow::clearGamepad(int deviceID) {
        if (deviceID < 0 || deviceID >= static_cast<int>(gamepadIDs.size())) {
            spdlog::error("Invalid device ID: {}", deviceID);
            return;
        }
        const auto idIterator = std::ranges::find(gamepadIDs, deviceID);
        if (idIterator == gamepadIDs.end()) {
            spdlog::error("Gamepad with device ID {} not found", deviceID);
            return;
        }
        const auto deviceIndex = static_cast<EDeviceID>(std::distance(gamepadIDs.begin(), idIterator));
        if (const auto gamepadIterator = gamepads.find(deviceIndex); gamepadIterator != gamepads.end()) {
            spdlog::info("Gamepad disconnected: {}", +deviceIndex);
            SDL_CloseGamepad(gamepadIterator->second);
            gamepads.erase(gamepadIterator);
            controllerState.erase(deviceIndex);
            gamepadIDs[static_cast<size_t>(deviceID)] = -1;
            if (controllerState.erase(deviceIndex)) {
                if (callbacks.OnControllerDisconnected) {
                    callbacks.OnControllerDisconnected(+deviceIndex);
                }
            }
        }
    }
} // namespace OZZ::platform::SDL3

#endif
