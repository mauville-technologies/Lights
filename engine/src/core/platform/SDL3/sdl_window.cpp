//
// Created by ozzadar on 2025-06-18.
//

#include "sdl_window.h"

#include "sdl_keys.h"

#ifdef OZZ_SDL3

// Vulkan (volk + SDL_vulkan) is desktop-only. On web the WebGPU backend builds
// its surface from the page canvas, so no Vulkan surface/extension code is used.
#if !defined(__EMSCRIPTEN__)
#include "SDL3/SDL_vulkan.h"
#include <volk.h>
#endif
#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>

#include <algorithm>

#include <spdlog/spdlog.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE void ozzBrowserPasted(const char* text) {
    SDL_SetClipboardText(text);
}

// SDL cancels the default of every Ctrl chord (so the browser never fires "paste") and its web clipboard is app-local.
// Let Ctrl/Cmd+V through and copy what the browser pastes into SDL's clipboard before the next frame reads the key.
EM_JS_DEPS(ozzBrowserPaste, "$stringToNewUTF8,free");
EM_JS(void, ozzInstallBrowserPaste, (), {
    window.addEventListener("keydown", (event) => {
        const isV = event.code === "KeyV" || event.key === "v" || event.key === "V";
        if ((event.ctrlKey || event.metaKey) && isV)
            event.preventDefault = () => {};
    }, true);
    window.addEventListener("paste", (event) => {
        const text = event.clipboardData ? event.clipboardData.getData("text/plain") : "";
        const ptr = stringToNewUTF8(text);
        _ozzBrowserPasted(ptr);
        _free(ptr);
    });
});
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
#if defined(__EMSCRIPTEN__)
        static const bool bPasteInstalled = (ozzInstallBrowserPaste(), true);
        (void)bPasteInstalled;
#endif
        bIsValid = true;
    }

    bool SDLWindow::CreateSurface(void* instance, void* surfaceOut) {
#if defined(__EMSCRIPTEN__)
        // No Vulkan on web; the WebGPU backend creates its surface from the canvas.
        (void)instance;
        (void)surfaceOut;
        return false;
#else
        // Vulkan-only. The WebGPU backend builds its own surface from the native handles
        // returned by GetNativeWindowHandles().
        if (!SDL_Vulkan_CreateSurface(
                window, static_cast<VkInstance>(instance), nullptr, static_cast<VkSurfaceKHR*>(surfaceOut))) {
            spdlog::error("Failed to create Vulkan surface: {}", SDL_GetError());
            return false;
        }
        return true;
#endif
    }

    OZZ::rendering::NativeWindowHandles SDLWindow::GetNativeWindowHandles() {
        OZZ::rendering::NativeWindowHandles handles{};
        SDL_PropertiesID props = SDL_GetWindowProperties(window);

#ifdef OZZ_PLATFORM_WINDOWS
        void* hwnd = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        if (!hwnd) {
            spdlog::error("Failed to get HWND from SDL window for native window handles");
            return handles;
        }
        handles.Platform = OZZ::rendering::NativeWindowHandles::Platform::Win32;
        handles.Display = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
        handles.Window = hwnd;
#elif defined(OZZ_PLATFORM_LINUX)
        // Try Wayland first, fall back to X11
        void* wlDisplay = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
        void* wlSurface = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
        if (wlDisplay && wlSurface) {
            handles.Platform = OZZ::rendering::NativeWindowHandles::Platform::Wayland;
            handles.Display = wlDisplay;
            handles.Window = wlSurface;
        } else {
            void* x11Display = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
            uint64_t x11Window =
                static_cast<uint64_t>(SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
            if (!x11Display || !x11Window) {
                spdlog::error("Failed to get native window handle (no Wayland or X11)");
                return handles;
            }
            handles.Platform = OZZ::rendering::NativeWindowHandles::Platform::X11;
            handles.Display = x11Display;
            handles.WindowId = x11Window;
        }
#else
        spdlog::error("Native window handles not implemented for this platform");
#endif
        return handles;
    }

    std::vector<std::string> SDLWindow::GetRequiredInstanceExtensions() {
#if defined(__EMSCRIPTEN__)
        // Web: WebGPU only, no Vulkan instance extensions.
        return {};
#else
#ifdef OZZ_WEBGPU_ENABLED
        if (rhiBackend == OZZ::rendering::RHIBackend::WebGPU)
            return {};
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
#endif // __EMSCRIPTEN__
    }

    void SDLWindow::InitInput(WindowCallbacks&& inCallbacks) {
        callbacks = std::move(inCallbacks);
    }

    void SDLWindow::Poll() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_GAMEPAD_ADDED: {
                    addGamepad(static_cast<int>(event.gdevice.which));
                    break;
                }
                case SDL_EVENT_GAMEPAD_REMOVED: {
                    // does it exist in the sdlGamepads list
                    clearGamepad(static_cast<int>(event.gdevice.which));

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
                        // Forward every byte of the UTF-8 text in order (one call per byte), so listeners that
                        // append bytes rebuild the full character. Forwarding only the first byte broke anything
                        // outside ASCII (e.g. accented letters).
                        for (const char* byte = event.text.text; *byte != '\0'; ++byte) {
                            callbacks.OnTextEvent(static_cast<unsigned char>(*byte));
                        }
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
                    // Also forward OS key-repeat so InputChord::bCanRepeat has events to act on.
                    if (auto oldKeyState = keyStates[sdlKey]; oldKeyState != newKeyState || event.key.repeat) {
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
                case SDL_EVENT_MOUSE_WHEEL: {
                    if (callbacks.OnMouseScroll) {
                        callbacks.OnMouseScroll(glm::vec2(event.wheel.x, event.wheel.y));
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

    std::string SDLWindow::GetClipboardText() const {
        char* text = SDL_GetClipboardText(); // never null; empty string when there is nothing
        std::string result = text ? text : "";
        SDL_free(text);
        return result;
    }

    void SDLWindow::SetClipboardText(const std::string& text) {
        if (!SDL_SetClipboardText(text.c_str())) {
            spdlog::warn("SDL_SetClipboardText failed: {}", SDL_GetError());
        }
    }

    void SDLWindow::SetTextMode(const bool bIsTextMode) {
        if (bIsTextMode) {
            SDL_StartTextInput(window);
        } else {
            SDL_StopTextInput(window);
        }
    }

    void SDLWindow::addGamepad(int instanceID) {
        if (std::ranges::find(gamepadIDs, instanceID) != gamepadIDs.end())
            return; // already connected (browsers can announce a pad more than once)
        const auto freeSlot = std::ranges::find(gamepadIDs, -1);
        if (freeSlot == gamepadIDs.end()) {
            spdlog::error("No available gamepad slots");
            return;
        }
        auto* newGamepad = SDL_OpenGamepad(instanceID);
        if (!newGamepad) {
            spdlog::error("Failed to open gamepad {}: {}", instanceID, SDL_GetError());
            return;
        }
#if defined(__EMSCRIPTEN__)
        // SDL's web default mapping makes every Gamepad API device a gamepad (pen tablets, wheels...); keep only
        // devices with at least the standard layout's 16 buttons and 4 axes.
        if (auto* joystick = SDL_GetGamepadJoystick(newGamepad);
            SDL_GetNumJoystickButtons(joystick) < 16 || SDL_GetNumJoystickAxes(joystick) < 4) {
            spdlog::info("Ignoring '{}': not a standard gamepad", SDL_GetJoystickName(joystick));
            SDL_CloseGamepad(newGamepad);
            return;
        }
#endif
        const auto deviceIndex = static_cast<EDeviceID>(std::distance(gamepadIDs.begin(), freeSlot));
        spdlog::info("Gamepad connected: {}", +deviceIndex);
        gamepads[deviceIndex] = newGamepad;
        controllerState[deviceIndex] = {};
        *freeSlot = instanceID;
        if (callbacks.OnControllerConnected) {
            callbacks.OnControllerConnected(+deviceIndex);
        }
    }

    void SDLWindow::clearGamepad(int instanceID) {
        const auto slot = std::ranges::find(gamepadIDs, instanceID);
        if (slot == gamepadIDs.end()) {
            spdlog::error("Gamepad with instance ID {} not found", instanceID);
            return;
        }
        const auto deviceIndex = static_cast<EDeviceID>(std::distance(gamepadIDs.begin(), slot));
        *slot = -1;
        if (const auto gamepadIterator = gamepads.find(deviceIndex); gamepadIterator != gamepads.end()) {
            SDL_CloseGamepad(gamepadIterator->second);
            gamepads.erase(gamepadIterator);
        }
        controllerState.erase(deviceIndex);
        spdlog::info("Gamepad disconnected: {}", +deviceIndex);
        if (callbacks.OnControllerDisconnected) {
            callbacks.OnControllerDisconnected(+deviceIndex);
        }
    }
} // namespace OZZ::platform::SDL3

#endif
