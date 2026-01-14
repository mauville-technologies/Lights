//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include <memory>
#include <toml.hpp>

#include "lights/audio/audio_subsystem.h"
#include "lights/input/input_subsystem.h"
#include "lights/platform/window.h"
#include "lights/rendering/renderer.h"
#include "lights/scene/scene.h"
#include "lights/util/configuration.h"

namespace OZZ::game {
    enum class EWindowMode { Windowed, BorderlessFullscreen };

    struct GameParameters {
        // Engine header
        float FPS{120.f};

        //  Window header
        EWindowMode WindowMode{EWindowMode::Windowed};
        glm::ivec2 WindowSize{1280, 720}; // Only used in Windowed mode

        uint32_t SampleRate{44100};
        uint8_t AudioChannels{2};

        bool fromToml(toml::basic_value<toml::type_config> data) {
            try {
                if (const auto EngineSection = data.at("Engine"); data.contains("Engine") && EngineSection.is_table()) {
                    if (EngineSection.contains("FPS") && EngineSection.at("FPS").is_floating()) {
                        FPS = static_cast<float>(EngineSection.at("FPS").as_floating());
                    }
                }

                if (const auto WindowSection = data.at("Window"); WindowSection.is_table()) {
                    if (WindowSection.contains("Mode") && WindowSection.at("Mode").is_integer()) {
                        WindowMode = static_cast<EWindowMode>(WindowSection.at("Mode").as_integer());
                    }
                    if (WindowSection.contains("Size") && WindowSection.at("Size").is_array()) {
                        const auto& sizeArray = WindowSection.at("Size");
                        if (sizeArray.size() == 2 && sizeArray[0].is_integer() && sizeArray[1].is_integer()) {
                            WindowSize.x = static_cast<int>(sizeArray[0].as_integer());
                            WindowSize.y = static_cast<int>(sizeArray[1].as_integer());
                        }
                    }
                }

                if (data.contains("Audio")) {
                    const auto AudioSection = data.at("Audio");
                    if (AudioSection.contains("SampleRate") && AudioSection.at("SampleRate").is_integer()) {
                        SampleRate = static_cast<uint32_t>(AudioSection.at("SampleRate").as_integer());
                    }
                    if (AudioSection.contains("Channels") && AudioSection.at("Channels").is_integer()) {
                        AudioChannels = static_cast<uint8_t>(AudioSection.at("Channels").as_integer());
                    }
                }
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
            value["Audio"]["SampleRate"] = SampleRate;
            value["Audio"]["Channels"] = AudioChannels;
            return value;
        }
    };

    template <typename SceneType>
    class LightsGame {
    public:
        explicit LightsGame(const std::filesystem::path& configFilePath)
            : params(Configuration<GameParameters>(configFilePath)) {}

        ~LightsGame() {
            scene.reset();
            renderer.reset();
            window.reset();
            input.reset();
        }

        void Run() {
            bRunning = true;

            initInput();
            initWindow();
            initAudio();
            initGL();
            initRenderer();
            initScene();

            auto lastTickTime = std::chrono::high_resolution_clock::now();
            auto lastRenderTime = std::chrono::high_resolution_clock::now();
            const auto renderRate = std::chrono::duration<float>(1.0f / params.Config.FPS);

            while (bRunning) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                if (scene->HasSceneEnded()) {
                    bRunning = false;
                    continue;
                }

                resourceManager->Tick();
                {
                    // Tick and render all scenes
                    auto deltaTime = std::chrono::duration<float>(currentTime - lastTickTime).count();

                    // in situations where the tick gets artificially slowed (Steamdeck pause, window move / resize), we
                    // don't want to have a ridiculous DeltaTime gap.
                    if (deltaTime > 1.f) {
                        deltaTime = 0.f;
                    }

                    scene->Tick(deltaTime);

                    if (currentTime - lastRenderTime >= renderRate) {
                        drawScene(scene.get());
                        lastRenderTime = currentTime;
                    }
                    lastTickTime = currentTime;
                }
                window->PollEvents();

                // Tick on all polled events
                if (input) {
                    input->Tick(window->GetKeyStates(), window->GetControllerState(), window->GetMouseButtonStates());
                }
                auto frameTime = std::chrono::high_resolution_clock::now();
                const double sleepsSec = 1.0 / params.Config.FPS - (frameTime - currentTime).count() / 1e9;
                preciseSleep(sleepsSec);
            }
        }

    private:
        void initWindow() {
            OZZ::platform::WindowCallbacks callbacks{.OnWindowClose =
                                                         [this]() {
                                                             bRunning = false;
                                                         },
                                                     .OnWindowResized =
                                                         [this](glm::ivec2 size) {
                                                             updateViewport(size);
                                                             if (scene) {
                                                                 scene->WindowResized(size);
                                                                 drawScene(scene.get());
                                                             }
                                                         },
                                                     .OnKeyPressed =
                                                         [this](InputKey key, EKeyState state) {
                                                             input->NotifyInputEvent({key, state});
                                                         },
                                                     .OnTextEvent =
                                                         [this](unsigned int unicode) {
                                                             // convert unicode to char
                                                             char character = static_cast<char>(unicode);
                                                             input->NotifyTextEvent(character);
                                                         },
                                                     .OnMouseMove =
                                                         [this](const glm::vec2 pos) {
                                                             input->NotifyMouseMove(pos);
                                                         }};
            window = std::make_shared<Window>(std::move(callbacks));
            input->SetTextModeFunc([this](bool bIsTextMode) {
                window->SetTextMode(bIsTextMode);
            });

            if (params.Config.WindowMode == EWindowMode::BorderlessFullscreen) {
                window->SetFullscreen(true);
            } else {
                window->SetFullscreen(false);
                window->SetWindowedSize(params.Config.WindowSize);
            }
        }

        void initAudio() {
            audio = std::make_shared<lights::audio::AudioSubsystem>();
            audio->Init();
        }

        void initGL() {
            // set up the viewport
            const auto size = window->GetSize();
            updateViewport(size);
        }

        void initInput() { input = std::make_shared<InputSubsystem>(); }

        void initRenderer() {
            renderer = std::make_unique<Renderer>();
            renderer->Init();

            resourceManager = std::make_unique<scene::ResourceManager>();
        }

        void initScene() {
            scene = std::make_unique<SceneType>();
            scene->InitScene(input, resourceManager.get());
            scene->WindowResized(window->GetSize());
        }

        // TODO: The viewport will probably live in a render target
        void updateViewport(glm::ivec2 size) { glViewport(0, 0, size.x, size.y); }

        void drawScene(OZZ::scene::Scene* scene) {
            // renderer->RenderScene(scene);
            window->MakeContextCurrent();
            renderer->ExecuteSceneGraph(scene->GetSceneGraph());
            if (scene == this->scene.get()) {
                window->SwapBuffers();
            }
        }

        void preciseSleep(double seconds) {
            // ref: https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
            using namespace std;
            using namespace std::chrono;

            static double estimate = 5e-3;
            static double mean = 5e-3;
            static double m2 = 0;
            static int64_t count = 1;

            while (seconds > estimate) {
                auto start = high_resolution_clock::now();
                this_thread::sleep_for(milliseconds(1));
                auto end = high_resolution_clock::now();

                double observed = (end - start).count() / 1e9;
                seconds -= observed;

                ++count;
                const double delta = observed - mean;
                mean += delta / count;
                m2 += delta * (observed - mean);
                const double stddev = sqrt(m2 / (count - 1));
                estimate = mean + stddev;
            }

            // spin lock
            const auto start = high_resolution_clock::now();
            while ((high_resolution_clock::now() - start).count() / 1e9 < seconds) {
            }
        }

    private:
        bool bRunning{false};
        OZZ::Configuration<GameParameters> params;

        std::shared_ptr<Window> window{nullptr};
        std::shared_ptr<InputSubsystem> input{nullptr};
        std::shared_ptr<lights::audio::AudioSubsystem> audio{nullptr};

        std::unique_ptr<OZZ::scene::Scene> scene{nullptr};
        std::unique_ptr<Renderer> renderer{nullptr};

        std::unique_ptr<scene::ResourceManager> resourceManager{nullptr};
    };
} // namespace OZZ::game
