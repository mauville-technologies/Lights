//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include <memory>
#include <toml.hpp>

#include "lights/platform/window.h"
#include "lights/rendering/renderer.h"
#include "lights/input/input_subsystem.h"
#include "lights/scene/scene.h"
#include "lights/util/configuration.h"
#include "network/client.h"
#include "application_state.h"

namespace OZZ::game {
    enum class EWindowMode {
        Windowed,
        BorderlessFullscreen
    };

    struct GameParameters {
        // Engine header
        float FPS{120.f};

        //  Window header
        EWindowMode WindowMode{EWindowMode::Windowed};
        glm::ivec2 WindowSize{1280, 720};  // Only used in Windowed mode
        // Add more parameters here as needed

        bool fromToml(toml::basic_value<toml::type_config> data) {
            try {
                FPS = static_cast<float>(data.at("Engine").at("FPS").as_floating());
                WindowMode = static_cast<EWindowMode>(data.at("Window").at("Mode").as_integer());
                WindowSize.x = static_cast<int>(data.at("Window").at("Size").at(0).as_integer());
                WindowSize.y = static_cast<int>(data.at("Window").at("Size").at(1).as_integer());
            } catch (...) {
                return false;
            }
            return true;
        }

        [[nodiscard]] toml::basic_value<toml::type_config> toToml() const {
            toml::basic_value<toml::type_config> value;
            value["Engine"]["FPS"] = FPS;
            value["Window"]["Mode"] = static_cast<int>(WindowMode);
            value["Window"]["Size"] = std::vector<int>{WindowSize.x, WindowSize.y};
            return value;
        }
    };

    template<typename SceneType>
    class ClientGame {
    public:
        explicit ClientGame(const std::filesystem::path& configFilePath) : params(configFilePath) {
        }

        ~ClientGame() {
            client->Stop();
            if (networkThread.joinable()) {
                networkThread.join();
            }
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
            const auto renderRate = std::chrono::duration<float>(1.0f / params.Config.FPS);

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
            
            if (params.Config.WindowMode == EWindowMode::BorderlessFullscreen) {
                window->SetFullscreen(true);
            } else {
                window->SetFullscreen(false);
                window->SetWindowedSize(params.Config.WindowSize);
            }

            window->OnWindowClose = [this]() {
                bRunning = false;
            };
            window->OnWindowResized = [this](glm::ivec2 size) {
                updateViewport(size);
                if (scene) {
                    scene->WindowResized(size);
                    drawScene(scene.get());
                }
            };

            window->OnKeyPressed = [this](InputKey key, EKeyState state) {
                input->NotifyInputEvent({key, state});
            };

            window->OnTextEvent = [this](unsigned int unicode) {
                // convert unicode to char
                char character = static_cast<char>(unicode);
                input->NotifyTextEvent(character);
            };

            window->OnMouseMove = [this](const glm::vec2 pos) {
                input->NotifyMouseMove(pos);
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
            scene->Init(input);
            scene->WindowResized(window->GetSize());
        }

        void initRenderer() {
            renderer = std::make_unique<Renderer>();
        }

        void initUI() {
            //TODO: UI abstraction will probably exist at some point
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
            if (scene == this->scene.get()) {
                window->SwapBuffers();
            }
        }

    private:
        bool bRunning{false};
        OZZ::Configuration<GameParameters> params;

        ApplicationState appState;

        std::shared_ptr<Window> window { nullptr };
        std::shared_ptr<InputSubsystem> input { nullptr };

        std::unique_ptr<OZZ::scene::Scene> scene { nullptr };
        std::unique_ptr<Renderer> renderer { nullptr };

        std::thread networkThread;
        std::unique_ptr<network::client::Client> client { nullptr };
    };
} // OZZ
