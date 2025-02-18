//
// Created by ozzadar on 2025-02-11.
//

#include "game_layer.h"
#include "game/objects/pepe.h"
#include "game/objects/tilemap.h"

namespace OZZ::game::scene {
    GameLayer::GameLayer(World* inWorld) : world(inWorld) {

    }

    GameLayer::~GameLayer() {
        if (world) {
            world->RemoveObject(pepe.first);
            // world->RemoveObject(ground.first);
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

        // Create a pepe
        auto goPepe = world->CreateGameObject<Pepe>();
        pepe = {goPepe.first, reinterpret_cast<Pepe*>(goPepe.second)};
        pepe.second->GetSceneObjects()[0].Transform = glm::scale(glm::mat4{1.f}, glm::vec3(64.f, 64.f, 1.0f));

        // auto goGround = world->CreateGameObject<GroundTest>();
        // ground = {goGround.first, reinterpret_cast<GroundTest*>(goGround.second)};

        auto goTilemap = world->CreateGameObject<Tilemap>();
        tilemap = {goTilemap.first, reinterpret_cast<Tilemap*>(goTilemap.second)};
        tilemap.second->Init("assets/map/test_map.tmj");
    }

    void GameLayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);

        pepe.second->Tick(DeltaTime);
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

        auto pepeObjects = pepe.second->GetSceneObjects();
        objects.insert(objects.end(), pepeObjects.begin(), pepeObjects.end());

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
                    pepe.second->MoveLeft();
                },
                .OnReleased = [this]() {
                    spdlog::info("Left Released");
                    pepe.second->StopMoving();
                }
            }
        });
        //
        inInput->RegisterInputMapping({
            .Action = "Right",
            .Chord = InputChord{.Keys = std::vector{EKey::Right}},
            .Callbacks = {
                .OnPressed = [this]() {
                    pepe.second->MoveRight();
                },
                .OnReleased = [this]() {
                    pepe.second->StopMoving();
                }
            }
        });
        inInput->RegisterInputMapping({
           .Action = "MoveUp",
              .Chord = InputChord{.Keys = std::vector<EKey>{EKey::Up}},
                .Callbacks = {
                        .OnPressed = [this]() {
                            if (pepe.second) {
                                spdlog::info("Jumping");
                                pepe.second->Jump();
                            }
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