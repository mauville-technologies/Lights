//
// Created by ozzadar on 2025-06-18.
//

#include "sdl_window.h"

#ifdef OZZ_SDL3

#include <spdlog/spdlog.h>

namespace OZZ::platform::SDL3 {
    void SDLWindow::CreateWindow(const std::string &title, int width, int height) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
            spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
            return;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
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

    void * SDLWindow::GetProcAddress() {
        return reinterpret_cast<void *>(SDL_GL_GetProcAddress);
    }

    void SDLWindow::InitInput(WindowCallbacks &&inCallbacks) {
        callbacks = std::move(inCallbacks);
    }

    void SDLWindow::Poll() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
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
                default:
                    break;
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

    void SDLWindow::SetFullscreen(bool fullscreen) {
        SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    }
} // OZZ

#endif