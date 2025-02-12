//
// Created by ozzadar on 2024-12-18.
//

#include "client_game.h"
#include "spdlog/spdlog.h"
#include "game/scenes/ingame/base_scene.h"
#include "scenes/physics_testing/physics_testing_scene.h"

namespace OZZ::game {
    ClientGame::ClientGame() : bRunning(false), appState() {

    }

    ClientGame::~ClientGame() {
        client->Stop();
        if (networkThread.joinable()) {
            networkThread.join();
        }
        ui->Shutdown();
        ui.reset();
        windowScene.reset();
        window.reset();
    }

    void ClientGame::Run() {
        bRunning = true;
        initInput();
        initWindow();
        initGL();
        initRenderer();
        initUI();
        initScene();
        initNetwork();

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

    void ClientGame::initRenderer() {
        renderer = std::make_unique<Renderer>();
    }

    void ClientGame::initUI() {
        ui = std::make_shared<UserInterface>();
        ui->Init(window->GetWindowHandle());
    }

    void ClientGame::initScene() {
        // windowScene = std::make_unique<scene::BaseScene>([this]() {
        //     return appState;
        // });
        windowScene = std::make_unique<scene::PhysicsTestingScene>();
        if (auto mainMenuScene = dynamic_cast<scene::BaseScene*>(windowScene.get())) {
            mainMenuScene->ConnectToServerRequested = [this]() {
                if (client) {
                    client->Stop();
                    if (networkThread.joinable()) {
                        networkThread.join();
                    }
                    initNetwork();
                }
            };

            mainMenuScene->DisconnectFromServerRequested = [this]() {
                client->Stop();
                if (networkThread.joinable()) {
                    networkThread.join();
                }
            };

            mainMenuScene->LoginRequested = [this](const std::string& username, const std::string& password) {
                client->Login(username, password);
            };

            mainMenuScene->LogoutRequested = [this]() {
                client->Stop();
                if (networkThread.joinable()) {
                    networkThread.join();
                }
            };
        }
        windowScene->Init(input, ui);
        windowScene->RenderTargetResized(window->GetSize());
    }

    void ClientGame::initNetwork() {
        client = std::make_unique<network::client::Client>();
        client->OnConnectedToServer = [this]() {
            spdlog::info("Connected to server!");
            appState.ConnectionState = ApplicationConnectionState::Connected;
        };

        client->OnConnectingToServer = [this]() {
            spdlog::info("Connecting to server...");
            appState.ConnectionState = ApplicationConnectionState::Connecting;
        };

        client->OnDisconnectedFromServer = [this]() {
            spdlog::info("Disconnected from server!");
            appState.ConnectionState = ApplicationConnectionState::Disconnected;
            appState.LoginState = ApplicationLoginState::NotLoggedIn;
            appState.CurrentPlayerState.Clear();
        };

        client->OnAuthenticationFailed = [this]() {
            spdlog::error("Authentication failed, exiting!");
            bRunning = false;
        };

        client->OnUserLoggedIn = [this](const network::messages::server::AuthenticationSuccessful& message) {
            spdlog::info("User logged in: {}", message.GetUsername());
            appState.LoginState = ApplicationLoginState::LoggedIn;
            appState.CurrentPlayerState.Username = message.GetUsername();
        };

        client->OnAccountLoggedInElsewhere = [this]() {
            spdlog::error("Account logged in elsewhere, exiting!");
            appState.LoginState = ApplicationLoginState::NotLoggedIn;
            appState.CurrentPlayerState.Clear();
        };

        networkThread = std::thread([this]() {
            client->Run("127.0.0.1", 1337);
        });
    }

    void ClientGame::updateViewport(glm::ivec2 size) {
        glViewport(0, 0, size.x, size.y);
    }

    void ClientGame::drawScene(Scene *scene) {
        renderer->RenderScene(scene);
        ui->Render();
        if (scene == windowScene.get()) {
            window->SwapBuffers();
        }
    }
} // OZZ