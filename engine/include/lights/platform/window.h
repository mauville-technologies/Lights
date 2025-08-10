//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <lights/platform/platform_window.h>
#include "glm/glm.hpp"
#include <memory>

namespace OZZ {
    class Window {
    public:
        Window(platform::WindowCallbacks&& inCallbacks);
        ~Window();

        void PollEvents();
        void MakeContextCurrent();
        void SwapBuffers();

        [[nodiscard]] glm::ivec2 GetSize() const;
        [[nodiscard]] const auto& GetKeyStates() const { return window->GetKeyStates(); }
        [[nodiscard]] const auto& GetControllerState() const { return window->GetControllerState(); }

        void SetFullscreen(bool bFullscreen);
        void SetWindowedSize(glm::ivec2 size);
        void SetTextMode(bool bIsTextMode);

    private:
        void initWindow(platform::WindowCallbacks&& inCallbacks);

    public:


    private:
        std::unique_ptr<platform::IPlatformWindow> window;

        inline static bool bGLADInitialized{false};
    };
} // OZZ
