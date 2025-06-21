//
// Created by ozzadar on 2025-06-18.
//

#pragma once

// #define OZZ_GLFW
#ifdef OZZ_GLFW
#include "lights/platform/platform_window.h"
#include <GLFW/glfw3.h>

namespace OZZ::platform::glfw {
    class GLFWWindow : public IPlatformWindow {
    public:
        ~GLFWWindow() override;

        void CreateWindow(const std::string &title, int width, int height) override;
        void * GetProcAddress() override;
        void InitInput(WindowCallbacks &&inCallbacks) override;
        void Poll() override;
        void MakeContextCurrent() override;
        void Present() override;
        [[nodiscard]] glm::ivec2 GetSize() const override;
        void SetSize(int width, int height) override;
        void SetFullscreen(bool fullscreen) override;
        void SetTextMode(bool bIsTextMode) override;
    private:
        void addController(int index);
        void removeController(int index);
        
    private:
        GLFWwindow* window { nullptr };

        WindowCallbacks callbacks;

    };
} // OZZ
#endif