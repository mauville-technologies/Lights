//
// Created by ozzadar on 2025-06-18.
//

#pragma once

#ifdef OZZ_SDL3

#include "lights/platform/platform_window.h"
#include <SDL3/SDL.h>

namespace OZZ::platform::SDL3 {
    class SDLWindow final : public IPlatformWindow {
    public:
        void CreateWindow(const std::string& title, int width, int height) override;
        void* GetProcAddress() override;
        void InitInput(WindowCallbacks&& inCallbacks) override;
        void Poll() override;
        void MakeContextCurrent() override;
        void Present() override;
        [[nodiscard]] glm::ivec2 GetSize() const override;
        void SetSize(int width, int height) override;
        void SetFullscreen(bool fullscreen) override;
        void SetTextMode(bool bIsTextMode) override;

        [[nodiscard]] bool IsValid() const override { return bIsValid; };

    private:
        void addGamepad(int sdlIndex);
        void clearGamepad(int sdlIndex);

    private:
        SDL_Window* window{nullptr};
        SDL_GLContext glContext{nullptr};

        WindowCallbacks callbacks;

        std::unordered_map<EDeviceID, SDL_Gamepad*> gamepads{};
        std::array<int, static_cast<size_t>(EDeviceID::NumberOfGamePads)> gamepadIDs{-1};
        bool bIsValid{false};
    };
} // namespace OZZ::platform::SDL3

#endif