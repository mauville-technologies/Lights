//
// Created by ozzadar on 2025-02-11.
//

#include "game_layer.h"
#include "game/objects/tilemap.h"

namespace OZZ::game::scene {
    GameLayer::GameLayer(GameWorld* inWorld) : world(inWorld) {

    }

    GameLayer::~GameLayer() {
        if (world) {
            world->RemoveObject(tilemap.first);
        }
    }

    void GameLayer::SetInputSubsystem(const std::shared_ptr<InputSubsystem>& inInput) {
        unregisterMappings(inInput);
        registerMappings(inInput);
        input = inInput;
    }

    void GameLayer::Init() {
        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f),  // Camera position
                                         glm::vec3(0.f, 0.f, 0.f),  // Target to look at
                                         glm::vec3(0.f, 1.f, 0.f)); // Up vector

        auto goTilemap = world->CreateGameObject<Tilemap>();
        tilemap = {goTilemap.first, reinterpret_cast<Tilemap*>(goTilemap.second)};
        tilemap.second->Init("assets/map/test_map.tmj");
    }

    void GameLayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);

        // ground.second->Tick(DeltaTime);
    }

    void GameLayer::RenderTargetResized(glm::ivec2 size) {
        spdlog::info("Main Menu Scene Resized to: {}x{}", size.x, size.y);
        auto width = size.x;
        auto height = size.y;

        LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.1f, 100.f);
    }

    std::vector<SceneObject> GameLayer::GetSceneObjects() {
        std::vector<SceneObject> objects;
        auto t = tilemap.second->GetSceneObjects();
        objects.insert(objects.end(), t.begin(), t.end());

        return objects;
    }

    void GameLayer::unregisterMappings(std::shared_ptr<InputSubsystem> inInput) {
        if (!inInput) {
            return;
        }

        inInput->UnregisterInputMapping("MoveUp");
        inInput->UnregisterInputMapping("MoveDown");
        inInput->UnregisterInputMapping("Quit");
        inInput->UnregisterInputMapping("Konami");
    }

    void GameLayer::registerMappings(std::shared_ptr<InputSubsystem> inInput) {
        if (!inInput) {
            return;
        }
        //
        inInput->RegisterInputMapping({
            .Action = "Left",
            .Chord = InputChord{.Keys = std::vector<EKey>{EKey::Left}},
            .Callbacks = {
                .OnPressed = [this]() {
                    spdlog::info("Left Pressed");
                },
                .OnReleased = [this]() {
                    spdlog::info("Left Released");
                }
            }
        });
        //
        inInput->RegisterInputMapping({
            .Action = "Right",
            .Chord = InputChord{.Keys = std::vector{EKey::Right}},
            .Callbacks = {
                .OnPressed = [this]() {
                },
                .OnReleased = [this]() {
                }
            }
        });
        inInput->RegisterInputMapping({
           .Action = "MoveUp",
              .Chord = InputChord{.Keys = std::vector<EKey>{EKey::Up}},
                .Callbacks = {
                        .OnPressed = [this]() {

                        },
                        .OnReleased = [this]() {
                            spdlog::info("Stop Up");
                        }
                }
        });

        inInput->RegisterInputMapping({
                .Action = "MoveDown",
                .Chord = InputChord{.Keys = std::vector<EKey>{EKey::Down}},
                .Callbacks = {
                        .OnPressed = [this]() {
                        },
                        .OnReleased = [this]() {
                            spdlog::info("Stop Up");
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
} // OZZ