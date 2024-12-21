//
// Created by ozzadar on 2024-12-18.
//

#include "client_game.h"
#include "spdlog/spdlog.h"
#include "game/scene/main_menu/main_menu_scene.h"

namespace OZZ {
    ClientGame::ClientGame() {

    }

    ClientGame::~ClientGame() {
        window.reset();
    }

    void ClientGame::Run() {
        bRunning = true;
        initInput();
        initWindow();
        initGL();
        initScene();
        initRenderer();

        auto lastTickTime = std::chrono::high_resolution_clock::now();
        auto lastRenderTime = std::chrono::high_resolution_clock::now();
        auto FPS = 60.f;
        auto renderRate = std::chrono::duration<float>(1.0f / FPS);

        while (bRunning) {
            {
                auto currentTime = std::chrono::high_resolution_clock::now();
                // Tick and render all scenes
                windowScene->Tick(std::chrono::duration<float>(currentTime - lastTickTime).count());

                if (currentTime - lastRenderTime >= renderRate) {
                    drawScene(windowScene.get());
                    lastRenderTime = currentTime;
                }
                lastTickTime = currentTime;
            }
            window->PollEvents();
        }
    }

    void ClientGame::initInput() {
        input = std::make_shared<InputSubsystem>();
    }

    void ClientGame::initWindow() {
        window = std::make_shared<Window>();
        window->OnWindowClose = [this]() {
            bRunning = false;
        };
        window->OnWindowResized = [this](glm::ivec2 size) {
            updateViewport(size);
            if (windowScene) {
                windowScene->RenderTargetResized(size);
                drawScene(windowScene.get());
            }
        };

        window->OnKeyPressed = [this](EKey key, EKeyState state) {
            input->NotifyKeyboardEvent({ key, state });
        };
    }

    void ClientGame::initGL() {
        // set up the viewport
        auto size = window->GetSize();
        updateViewport(size);
    }


    void ClientGame::initScene() {
        windowScene = std::make_unique<MainMenuScene>();
        windowScene->Init(input);
        windowScene->RenderTargetResized(window->GetSize());
    }

    void ClientGame::initRenderer() {
        renderer = std::make_unique<Renderer>();
    }

    void ClientGame::updateViewport(glm::ivec2 size) {
        glViewport(0, 0, size.x, size.y);
    }

    void ClientGame::drawScene(Scene *scene) {
        renderer->RenderScene(scene);

        if (scene == windowScene.get()) {
            window->SwapBuffers();
        }
    }


} // OZZ