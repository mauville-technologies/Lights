//
// Created by ozzadar on 2025-06-18.
//

#include "sdl_window.h"

#ifdef OZZ_SDL3

namespace OZZ::platform::SDL3 {
    void SDLWindow::CreateWindow(const std::string &title, int width, int height) {
    }

    void * SDLWindow::GetProcAddress() {
        return nullptr;
    }

    void SDLWindow::InitInput(InputCallbacks &&callbacks) {
    }

    void SDLWindow::Poll() {
    }

    void SDLWindow::MakeContextCurrent() {
    }

    void SDLWindow::Present() {
    }

    glm::ivec2 SDLWindow::GetSize() const {
        return glm::ivec2{0, 0}; // Placeholder, implement actual size retrieval
    }

    void SDLWindow::SetSize(int width, int height) {

    }

    void SDLWindow::SetFullscreen(bool fullscreen) {
    }
} // OZZ

#endif