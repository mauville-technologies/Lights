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
        initWindow();
        initGL();
        initScene();
        initRenderer();

        auto lastTickTime = std::chrono::high_resolution_clock::now();
        auto FPS = 60.f;
        auto renderRate = std::chrono::duration<float>(1.0f / FPS);

        while (bRunning) {
            {
                auto currentTime = std::chrono::high_resolution_clock::now();
                // Tick and render all scenes
                windowScene->Tick(0.f);

                if (currentTime - lastTickTime >= renderRate) {
                    lastTickTime = currentTime;
                    renderer->RenderScene(windowScene.get());

                    // TODO: This is obviously always true but keep it here as a reminder for later when I can have multiple scenes rendering.
                    if (windowScene == windowScene) {
                        window->SwapBuffers();
                    }
                }

            }
            window->PollEvents();
        }
    }

    void ClientGame::initWindow() {
        window = std::make_shared<Window>();
        window->OnWindowClose = [this]() {
            bRunning = false;
        };
        window->OnWindowResized = [this](glm::ivec2 size) {
            updateViewport(size);
        };
    }

    void ClientGame::initGL() {
        // set up the viewport
        auto size = window->GetSize();
        updateViewport(size);
    }

    void ClientGame::initScene() {
        windowScene = std::make_unique<MainMenuScene>();
    }

    void ClientGame::initRenderer() {
        renderer = std::make_unique<Renderer>();
    }

    void ClientGame::updateViewport(glm::ivec2 size) {
        glViewport(0, 0, size.x, size.y);
    }

} // OZZ