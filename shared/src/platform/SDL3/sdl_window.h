//
// Created by ozzadar on 2025-06-18.
//

#pragma once

#ifdef OZZ_SDL3

#include "lights/platform/platform_window.h"

namespace OZZ::platform::SDL3 {
    class SDLWindow final : public IPlatformWindow {
    public:
        void CreateWindow(const std::string &title, int width, int height) override;
        void * GetProcAddress() override;
        void InitInput(InputCallbacks &&callbacks) override;
        void Poll() override;
        void MakeContextCurrent() override;
        void Present() override;
        [[nodiscard]] glm::ivec2 GetSize() const override;
        void SetSize(int width, int height) override;
        void SetFullscreen(bool fullscreen) override;
    };
} // OZZ

#endif