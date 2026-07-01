//
// Created by ozzadar on 2025-06-18.
//

#include "sdl_window.h"

#include "sdl_keys.h"

#ifdef OZZ_SDL3

#include <volk.h>

#include "SDL3/SDL_vulkan.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>

#include <spdlog/spdlog.h>

#ifdef OZZ_WEBGPU_ENABLED
#include <dawn/webgpu.h>
#ifdef OZZ_PLATFORM_WINDOWS
#include <windows.h>
#undef CreateWindow  // windows.h macro conflicts with IPlatformWindow::CreateWindow
#endif
#endif

namespace OZZ::platform::SDL3 {
    void SDLWindow::CreateWindow(const std::string& title, int width, int height) {
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD) < 0) {
            spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
            return;
        }

        const SDL_WindowFlags windowFlags = rhiBackend == OZZ::rendering::RHIBackend::WebGPU
            ? SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN
            : SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
        window = SDL_CreateWindow(title.c_str(), width, height, windowFlags);
        if (!window) {
            spdlog::error("Failed to create SDL window: {}", SDL_GetError());
            return;
        }

        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        bIsValid = true;
    }

    bool SDLWindow::CreateSurface(void* instance, void* surfaceOut) {
#ifdef OZZ_WEBGPU_ENABLED
        if (rhiBackend == OZZ::rendering::RHIBackend::WebGPU) {
            WGPUInstance wgpuInstance = *static_cast<WGPUInstance*>(instance);
            WGPUSurface* surfacePtr   = static_cast<WGPUSurface*>(surfaceOut);

#ifdef OZZ_PLATFORM_WINDOWS
            void* hwnd = SDL_GetPointerProperty(
                SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
            if (!hwnd) {
                spdlog::error("Failed to get HWND from SDL window for WebGPU surface");
                return false;
            }
            WGPUSurfaceSourceWindowsHWND hwndDesc = {};
            hwndDesc.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
            hwndDesc.hinstance   = GetModuleHandle(nullptr);
            hwndDesc.hwnd        = hwnd;
            WGPUSurfaceDescriptor surfDesc = {};
            surfDesc.nextInChain = &hwndDesc.chain;
            *surfacePtr = wgpuInstanceCreateSurface(wgpuInstance, &surfDesc);
#elif defined(OZZ_PLATFORM_LINUX)
            // Try Wayland first, fall back to X11
            struct wl_display* wlDisplay = static_cast<struct wl_display*>(SDL_GetPointerProperty(
                SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr));
            struct wl_surface* wlSurface = static_cast<struct wl_surface*>(SDL_GetPointerProperty(
                SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr));
            if (wlDisplay && wlSurface) {
                WGPUSurfaceSourceWaylandSurface wlDesc = {};
                wlDesc.chain.sType = WGPUSType_SurfaceSourceWaylandSurface;
                wlDesc.display     = wlDisplay;
                wlDesc.surface     = wlSurface;
                WGPUSurfaceDescriptor surfDesc = {};
                surfDesc.nextInChain = &wlDesc.chain;
                *surfacePtr = wgpuInstanceCreateSurface(wgpuInstance, &surfDesc);
            } else {
                void* x11Display = SDL_GetPointerProperty(
                    SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
                uint64_t x11Window = static_cast<uint64_t>(SDL_GetNumberProperty(
                    SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
                if (!x11Display || !x11Window) {
                    spdlog::error("Failed to get native window handle for WebGPU surface (no Wayland or X11)");
                    return false;
                }
                WGPUSurfaceSourceXlibWindow xlibDesc = {};
                xlibDesc.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
                xlibDesc.display     = x11Display;
                xlibDesc.window      = x11Window;
                WGPUSurfaceDescriptor surfDesc = {};
                surfDesc.nextInChain = &xlibDesc.chain;
                *surfacePtr = wgpuInstanceCreateSurface(wgpuInstance, &surfDesc);
            }
#else
            spdlog::error("WebGPU surface creation not implemented for this platform");
            return false;
#endif
            if (!*surfacePtr) {
                spdlog::error("wgpuInstanceCreateSurface returned null");
                return false;
            }
            return true;
        }
#endif
        // Vulkan path
        if (!SDL_Vulkan_CreateSurface(
                window, static_cast<VkInstance>(instance), nullptr, static_cast<VkSurfaceKHR*>(surfaceOut))) {
            spdlog::error("Failed to create Vulkan surface: {}", SDL_GetError());
            return false;
        }
        return true;
    }

    std::vector<std::string> SDLWindow::GetRequiredInstanceExtensions() {
#ifdef OZZ_WEBGPU_ENABLED
        if (rhiBackend == OZZ::rendering::RHIBackend::WebGPU) return {};
#endif
        uint32_t count;
        auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);
        std::vector<std::string> extensionList;
        if (extensions) {
            extensionList.reserve(count);
            for (uint32_t i = 0; i < count; i++) {
                extensionList.emplace_back(extensions[i]);
            }
        }
        return extensionList;
    }

    void SDLWindow::InitInput(WindowCallbacks&& inCallbacks) {
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
                    if (auto& controller = controllerState[deviceID]; controller[button] != newButtonState) {
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

        for (const auto& [deviceID, gamepad] : gamepads) {
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

    void SDLWindow::Show() {
        SDL_ShowWindow(window);
    }

    void SDLWindow::SetTextMode(const bool bIsTextMode) {
        if (bIsTextMode) {
            SDL_StartTextInput(window);
        } else {
            SDL_StopTextInput(window);
        }
    }

    void SDLWindow::addGamepad(int sdlIndex) {
        if (auto* newGamepad = SDL_OpenGamepad(sdlIndex)) {
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
