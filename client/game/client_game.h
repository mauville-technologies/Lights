//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include <memory>
#include "lights/platform/window.h"
#include "lights/rendering/renderer.h"
#include "lights/input/input_subsystem.h"
#include "lights/scene/scene.h"
#include "lights/ui/user_interface.h"
#include "network/client.h"
#include "application_state.h"

namespace OZZ::game {
    template<typename SceneType>
    class ClientGame {
    public:
        ClientGame() = default;

        ~ClientGame() {
            client->Stop();
            if (networkThread.joinable()) {
                networkThread.join();
            }
            ui->Shutdown();
            ui.reset();
            scene.reset();
            window.reset();
        }

        void Run() {
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
            const auto FPS = 120.f;
            const auto renderRate = std::chrono::duration<float>(1.0f / FPS);

            while (bRunning) {
                {
                    auto currentTime = std::chrono::high_resolution_clock::now();
                    // Tick and render all scenes
                    scene->Tick(std::chrono::duration<float>(currentTime - lastTickTime).count());

                    if (currentTime - lastRenderTime >= renderRate) {
                        drawScene(scene.get());
                        lastRenderTime = currentTime;
                    }
                    lastTickTime = currentTime;
                }
                window->PollEvents();

                // Tick on all polled events
                if (input) {
                    input->Tick(window->GetKeyStates(), window->GetControllerState());
                }
            }
        }

    private:
        void initWindow() {
            window = std::make_shared<Window>();
            window->OnWindowClose = [this]() {
                bRunning = false;
            };
            window->OnWindowResized = [this](glm::ivec2 size) {
                updateViewport(size);
                if (scene) {
                    scene->RenderTargetResized(size);
                    drawScene(scene.get());
                }
            };

            window->OnKeyPressed = [this](InputKey key, EKeyState state) {
                input->NotifyInputEvent({key, state});
            };
        }

        void initGL() {
            // set up the viewport
            const auto size = window->GetSize();
            updateViewport(size);
        }

        void initInput() {
            input = std::make_shared<InputSubsystem>();
        }

        void initScene() {
            scene = std::make_unique<SceneType>();
            scene->Init(input, ui);
            scene->RenderTargetResized(window->GetSize());
        }

        void initRenderer() {
            renderer = std::make_unique<Renderer>();
        }

        void initUI() {
            ui = std::make_shared<UserInterface>();
            ui->Init(window->GetWindowHandle());
        }

        void initNetwork() {
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

            client->OnUserLoggedIn = [this](const network::messages::server::AuthenticationSuccessful &message) {
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

        // TODO: The viewport will probably live in a render target
        void updateViewport(glm::ivec2 size) {
            glViewport(0, 0, size.x, size.y);
        }

        void drawScene(OZZ::scene::Scene *scene) {
            renderer->RenderScene(scene);
            ui->Render();
            if (scene == this->scene.get()) {
                window->SwapBuffers();
            }
        }

    private:
        bool bRunning{false};

        ApplicationState appState;

        std::shared_ptr<Window> window { nullptr };
        std::shared_ptr<InputSubsystem> input { nullptr };
        std::shared_ptr<UserInterface> ui { nullptr };

        std::unique_ptr<OZZ::scene::Scene> scene { nullptr };
        std::unique_ptr<Renderer> renderer { nullptr };

        std::thread networkThread;
        std::unique_ptr<network::client::Client> client { nullptr };
    };
} // OZZ
