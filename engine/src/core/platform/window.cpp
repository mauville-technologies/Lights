//
// Created by ozzadar on 2024-12-18.
//

#include "lights/core/platform/window.h"
#include "lights/core/util/profiling.h"
#include <spdlog/spdlog.h>

#include "core/platform/SDL3/sdl_window.h"

namespace OZZ {
    Window::Window(platform::WindowCallbacks&& inCallbacks, rendering::RHIBackend backend) {
        initWindow(std::move(inCallbacks), backend);
    }

    Window::~Window() {
        window.reset();
    }

    void Window::initWindow(platform::WindowCallbacks&& inCallbacks, rendering::RHIBackend backend) {
        window = std::make_unique<platform::SDL3::SDLWindow>();

        window->SetRHIBackend(backend);
        window->CreateWindow("Ozzadar", 800, 600);
        window->MakeContextCurrent();

        window->InitInput(std::move(inCallbacks));
    }

    void Window::PollEvents() {
        OZZ_PROFILE_FUNCTION;
        window->Poll();

        // TODO: Add mouse and joystick input
    }

    void Window::MakeContextCurrent() {}

    void Window::SwapBuffers() {
        OZZ_PROFILE_FUNCTION;
        window->Present();
    }

    glm::ivec2 Window::GetSize() const {
        return window->GetSize();
    }

    void Window::SetFullscreen(bool bFullscreen) {
        window->SetFullscreen(bFullscreen);
    }

    void Window::SetWindowedSize(glm::ivec2 size) {
        window->SetSize(size.x, size.y);
    }

    void Window::SetTextMode(bool bIsTextMode) {
        if (window) {
            window->SetTextMode(bIsTextMode);
        }
    }

} // namespace OZZ
