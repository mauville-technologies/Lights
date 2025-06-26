//
// Created by ozzadar on 2024-12-18.
//

#include "lights/platform/window.h"
#include <spdlog/spdlog.h>

#ifdef OZZ_GLFW
#include "platform/glfw/glfw_window.h"
#endif

#ifdef OZZ_SDL3
#include "platform/sdl3/sdl_window.h"
#endif


namespace OZZ {
    Window::Window(platform::WindowCallbacks&& inCallbacks) {
        initWindow(std::move(inCallbacks));
    }

    Window::~Window() {
        window.reset();
    }

    void Window::initWindow(platform::WindowCallbacks&& inCallbacks) {
#ifdef OZZ_GLFW
        window = std::make_unique<platform::glfw::GLFWWindow>();
#endif

#ifdef OZZ_SDL3
        window = std::make_unique<platform::SDL3::SDLWindow>();
#endif

        window->CreateWindow("Ozzadar", 800, 600);
        window->MakeContextCurrent();

        if (!bGLADInitialized) {
            if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(window->GetProcAddress()))) {
                spdlog::error("Failed to initialize GLAD");
                return;
            }
            bGLADInitialized = true;
        }

        window->InitInput(std::move(inCallbacks));
    }

    void Window::PollEvents() {
        window->Poll();

        //TODO: Add mouse and joystick input
    }

    void Window::MakeContextCurrent() {

    }

    void Window::SwapBuffers() {
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
} // OZZ