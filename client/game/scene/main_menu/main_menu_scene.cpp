//
// Created by ozzadar on 2024-12-18.
//

#include "main_menu_scene.h"

#include <utility>
#include "game/scene/constants.h"
#include "game/scene/main_menu/objects/pepe.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "spdlog/spdlog.h"

namespace OZZ::game::scene {

    MainMenuScene::MainMenuScene(GetApplicationStateFunction inAppStateFunction, std::shared_ptr<InputSubsystem> inInput, std::shared_ptr<UserInterface> inUI)
        : appStateFunction(std::move(inAppStateFunction)), input(std::move(inInput)), ui(std::move(inUI)) {}

    MainMenuScene::~MainMenuScene() {
        input.reset();

        for (auto &Layer: Layers) {
            Layer->Objects.clear();
            Layer->Lights.clear();
        }

        Layers.clear();

        pepeLayer.reset();
        debugWindow.reset();
        ui.reset();
        if (world) world->DeInit();
        world.reset();
    }

    void MainMenuScene::Init() {
        world = std::make_shared<World>();
        world->Init({
            .Gravity = {0.f, -20.f * constants::UnitsPerMeter},
        });

        debugWindow = std::make_shared<ui::DebugWindow>(appStateFunction);

        if (auto typedDebugWindow = std::static_pointer_cast<ui::DebugWindow>(debugWindow)) {
            // this essentially passes through all the callbacks to the main menu scene
            typedDebugWindow->ConnectToServerRequested = ConnectToServerRequested;
            typedDebugWindow->DisconnectFromServerRequested = DisconnectFromServerRequested;
            typedDebugWindow->LoginRequested = LoginRequested;
            typedDebugWindow->LogoutRequested = LogoutRequested;
        }

        ui->AddComponent(debugWindow);

        pepeLayer = std::make_shared<PepeLayer>(world.get());
        pepeLayer->SetInputSubsystem(input);

        Layers.push_back(pepeLayer);

        for (auto &Layer: Layers) {
            Layer->Init();
        }
    }

    void MainMenuScene::Tick(float DeltaTime) {
        Scene::Tick(DeltaTime);

        // This will tick the physics world at a fixed rate
        static constexpr float physicsTickRate = 1.f / 60.f;
        static float accumulator = 0.f;
        accumulator += DeltaTime;

        while (accumulator >= physicsTickRate) {
            world->PhysicsTick(physicsTickRate);
            accumulator -= physicsTickRate;
        }
    }


    PepeLayer::PepeLayer(World* inWorld) : world(inWorld) {

    }

    void PepeLayer::SetInputSubsystem(const std::shared_ptr<InputSubsystem>& inInput) {
        unregisterMappings(inInput);
        registerMappings(inInput);
        input = inInput;
    }

    void PepeLayer::Init() {
        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f),  // Camera position
                                         glm::vec3(0.f, 0.f, 0.f),  // Target to look at
                                         glm::vec3(0.f, 1.f, 0.f)); // Up vector

        // Create a pepe
        auto goPepe = world->CreateGameObject<Pepe>();
        pepe = reinterpret_cast<Pepe*>(goPepe.second);
        Objects.push_back(pepe->GetSceneObject());

        pepe->GetSceneObject()->Transform = glm::scale(glm::mat4{1.f}, glm::vec3(64.f, 64.f, 1.0f));

        auto goGround = world->CreateGameObject<GroundTest>();
        ground = reinterpret_cast<GroundTest*>(goGround.second);
        Objects.push_back(ground->GetSceneObject());
    }

    void PepeLayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);
        auto MoveSpeed = 10.f; // pixels per second
        auto velocity = MoveSpeed * DeltaTime * glm::vec3(movement.x, movement.y, 0.f);
        pepe->GetSceneObject()->Transform = glm::translate(pepe->GetSceneObject()->Transform, velocity);

        pepe->Tick(DeltaTime);
        ground->Tick(DeltaTime);
    }

    void PepeLayer::RenderTargetResized(glm::ivec2 size) {
        spdlog::info("Main Menu Scene Resized to: {}x{}", size.x, size.y);
        auto width = size.x;
        auto height = size.y;

        LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.1f, 100.f);
    }

    void PepeLayer::ChangeDirection() {
        direction = !direction;
    }

    void PepeLayer::unregisterMappings(std::shared_ptr<InputSubsystem> inInput) {
        if (!inInput) {
            return;
        }

        inInput->UnregisterInputMapping("MoveUp");
        inInput->UnregisterInputMapping("MoveDown");
        inInput->UnregisterInputMapping("Quit");
        inInput->UnregisterInputMapping("Konami");
    }

    void PepeLayer::registerMappings(std::shared_ptr<InputSubsystem> inInput) {
        if (!inInput) {
            return;
        }

        inInput->RegisterInputMapping({
           .Action = "MoveUp",
              .Chord = InputChord{.Keys = std::vector<EKey>{EKey::Up}},
                .Callbacks = {
                        .OnPressed = [this]() {
                            if (pepe) {
                                spdlog::info("Jumping");
                                pepe->Jump();
                            }
                        },
                        .OnReleased = [this]() {
                            spdlog::info("Stop Up");
                            movement.y = 0;
                        }
                }
        });

        inInput->RegisterInputMapping({
                .Action = "MoveDown",
                .Chord = InputChord{.Keys = std::vector<EKey>{EKey::Down}},
                .Callbacks = {
                        .OnPressed = [this]() {

                            movement.y = -1;
                        },
                        .OnReleased = [this]() {
                            spdlog::info("Stop Up");
                            movement.y = 0;
                        }
                }
        });

        inInput->RegisterInputMapping({
                .Action = "Quit",
                .Chord = InputChord{.Keys = std::vector<EKey>{EKey::LControl, EKey::LAlt, EKey::X}},
                .Callbacks = {
                        .OnPressed = [this]() {
                            spdlog::info("Quit hotkey pressed");
                        },
                        .OnReleased = []() {
                            spdlog::info("Quit hotkey released");
                        }
                }
        });

        inInput->RegisterInputMapping({
                .Action = "Konami",
                .Chord = {
                        .Keys = std::vector<EKey>{EKey::Up, EKey::Up, EKey::Down, EKey::Down,
                                                  EKey::Left, EKey::Right, EKey::Left, EKey::Right,
                                                  EKey::B, EKey::A},
                        .bIsSequence = true,
                        .TimeBetweenKeys = 1000ms

                },
                .Callbacks = {
                        .OnPressed = []() {
                            spdlog::info("Konami Code Entered");
                        }
                }
        });
    }


    void UILayer::Init() {
    }

    void UILayer::RenderTargetResized(glm::ivec2 size) {

    }

    void UILayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);
    }
} // OZZ