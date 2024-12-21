//
// Created by ozzadar on 2024-12-18.
//

#include "main_menu_scene.h"
#include "game/scene/main_menu/objects/pepe.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "spdlog/spdlog.h"

namespace OZZ {

    void MainMenuScene::Init(std::shared_ptr<InputSubsystem> Input) {
        Scene::Init(Input);
        spdlog::info("Main Menu Scene Initialized");

        pepeLayer = std::make_shared<PepeLayer>();
        pepeLayer2 = std::make_shared<PepeLayer>();
        pepeLayer2->ChangeDirection();

        Layers.push_back(pepeLayer);
        Layers.push_back(pepeLayer2);

        for (auto &Layer: Layers) {
            Layer->Init(Input);
        }
    }

    void MainMenuScene::Tick(float DeltaTime) {
        Scene::Tick(DeltaTime);
    }

    PepeLayer::PepeLayer() {

    }

    void PepeLayer::Init(std::shared_ptr<InputSubsystem> Input) {
        SceneLayer::Init(Input);
        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f),  // Camera position
                                         glm::vec3(0.f, 0.f, 0.f),  // Target to look at
                                         glm::vec3(0.f, 1.f, 0.f)); // Up vector

        // Create a pepe
        pepe = std::make_shared<Pepe>();
        Objects.push_back(pepe);

        pepe->Transform = glm::scale(glm::mat4{1.f}, glm::vec3(64.f, 64.f, 1.0f));

        // scale da pepe
        // TODO: Multiple keys, scales per key
        input->RegisterInputMapping({
           .Action = "MoveUp",
              .Chord = InputChord{.Keys = std::vector<EKey>{EKey::Up}},
                .Callbacks = {
                        .OnPressed = [this]() {
                            spdlog::info("Move Up");
                            movement.y = 1;
                        },
                        .OnReleased = [this]() {
                            spdlog::info("Stop Up");
                            movement.y = 0;
                        }
                }
        });
        input->RegisterInputMapping({
                .Action = "MoveDown",
                .Chord = InputChord{.Keys = std::vector<EKey>{EKey::Down}},
                .Callbacks = {
                        .OnPressed = [this]() {
                            spdlog::info("Move Up");
                            movement.y = -1;
                        },
                        .OnReleased = [this]() {
                            spdlog::info("Stop Up");
                            movement.y = 0;
                        }
                }
        });
        input->RegisterInputMapping({
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

        input->RegisterInputMapping({
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

    void PepeLayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);
        auto MoveSpeed = 10.f; // pixels per second
        auto velocity = MoveSpeed * DeltaTime * glm::vec3(movement.x, movement.y, 0.f);
        pepe->Transform = glm::translate(pepe->Transform, velocity);
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

    void UILayer::Init(std::shared_ptr<InputSubsystem> Input) {
        SceneLayer::Init(Input);
    }

    void UILayer::RenderTargetResized(glm::ivec2 size) {

    }

    void UILayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);
    }
} // OZZ