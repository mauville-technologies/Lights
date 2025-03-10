//
// Created by ozzadar on 2025-02-12.
//

#include "physics_testing_layer.h"
#include "lights/game/game_object.h"

namespace OZZ::game::scene {
    PhysicsTestingLayer::PhysicsTestingLayer(GameWorld *inWorld) : gameWorld(inWorld) {
    }

    PhysicsTestingLayer::~PhysicsTestingLayer() {
        if (gameWorld) {
            gameWorld->RemoveObject(pepeid);
            gameWorld->RemoveObject(groundId);
            gameWorld->RemoveObject(rightWallId);
            gameWorld->RemoveObject(leftWallId);
            gameWorld->RemoveObject(topWallId);
        }
    }

    void PhysicsTestingLayer::Init() {
        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), // Camera position
                                             glm::vec3(0.f, 0.f, 0.f), // Target to look at
                                             glm::vec3(0.f, 1.f, 0.f)); // Up vector

        auto [id, inpepe] = gameWorld->CreateGameObject<Sprite>("assets/textures/pepe.png");
        // scale pepe
        pepeid = id;
        pepe = inpepe;
        pepe->AddBody(
            BodyType::Dynamic,
            OzzRectangle{
                .Position = glm::vec3{0, 2.f * constants::PixelsPerMeter, 0.f},
                .Size = glm::vec3{2.f * constants::PixelsPerMeter, 2.f * constants::PixelsPerMeter, 1.f}
            },
            glm::vec2{0, 0});


        auto [rId, inRightWall] = gameWorld->CreateGameObject<Sprite>("assets/textures/container.jpg");
        rightWall = inRightWall;
        rightWallId = rId;
        rightWall->AddBody(
            BodyType::Static,
            OZZ::collision::shapes::OzzRectangle{
                .Position = glm::vec3{6.f * constants::PixelsPerMeter, 0.f, 1.f},
                .Size = glm::vec3{1.f * constants::PixelsPerMeter, 12.f * constants::PixelsPerMeter, 1.f},
            },
            glm::vec2{0, 0});


        auto [lId, inLeftWall] = gameWorld->CreateGameObject<Sprite>("assets/textures/container.jpg");
        leftWallId = lId;
        leftWall = inLeftWall;
        leftWall->AddBody(
            BodyType::Static,
            OZZ::collision::shapes::OzzRectangle{
                .Position = glm::vec3{-6.f * constants::PixelsPerMeter, 0.f, 1.f},
                .Size = glm::vec3{1.f * constants::PixelsPerMeter, 12.f * constants::PixelsPerMeter, 1.f}
            },
            glm::vec2{0, 0});

        auto [tId, inTopWall] = gameWorld->CreateGameObject<Sprite>("assets/textures/container.jpg");
        topWallId = tId;
        topWall = inTopWall;
        topWall->AddBody(
            BodyType::Static,
            OZZ::collision::shapes::OzzRectangle{
                .Position = glm::vec3{0.f, 10.f * constants::PixelsPerMeter, 1.f},
                .Size = glm::vec3{12.f * constants::PixelsPerMeter, 1.f * constants::PixelsPerMeter, 1.f}
            },
            glm::vec2{0, 0});

        auto [gId, inGround] = gameWorld->CreateGameObject<Sprite>("assets/textures/container.jpg");
        groundId = gId;
        ground = inGround;
        ground->AddBody(
            BodyType::Static,
            OZZ::collision::shapes::OzzRectangle
            {
                .Position = glm::vec3{0.f, -2.f * constants::PixelsPerMeter, 1.f},
                .Size = glm::vec3{12.f * constants::PixelsPerMeter, 1.f * constants::PixelsPerMeter, 1.f}
            },
            glm::vec2{0, 0});

        // Set up jump button
        // TODO: expand input mappings to support multiple chords
        input->RegisterInputMapping({
            .Action = "Jump",
            .Chord = InputChord{.Keys = std::vector<InputKey>{{0,EControllerButton::A}}},
            // .Chord = InputChord{.Keys = std::vector<InputKey>{{-1,EKey::Space}}},
            .Callbacks = {
                .OnPressed = [this]() {
                    if (auto *body = pepe->GetBody()) {
                        body->Velocity.y = 20;
                    }
                },
                .OnReleased = [this]() {
                }
            }
        });

        input->RegisterAxisMapping({
            .Action = "MoveLeftRight",
            .Keys = {
                {{-1, EKey::Left}, -1.f},
                {{-1, EKey::Right}, 1.f},
                {{-1, EKey::A}, -1.f},
                {{-1, EKey::D}, 1.f},
                {{0, EControllerButton::LeftStickX}, 1.f},
            },
        });

        updateViewMatrix();
    }

    void PhysicsTestingLayer::PhysicsTick(float DeltaTime) {
        SceneLayer::PhysicsTick(DeltaTime);
    }

    void PhysicsTestingLayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);

        // TODO: I feel like the gameworld doesn't need to be owned by the layer -- though I'm not entirely sure of the alternative.
        // I'll leave it here for now and see if it makes sense to move it somewhere else (like the scene) further down the line
        // Doing it here makes it easy to forget in the future if
        gameWorld->Tick(DeltaTime);

        const auto moveValue = input->GetAxisValue("MoveLeftRight");
        if (auto *body = pepe->GetBody()) {
            body->Velocity.x = moveValue * 10;
        }

        updateViewMatrix();
    }

    void PhysicsTestingLayer::SetInputSubsystem(const std::shared_ptr<InputSubsystem> &inInput) {
        input = inInput;
    }

    void PhysicsTestingLayer::RenderTargetResized(glm::ivec2 size) {
        const auto width = size.x * 2;
        const auto height = size.y * 2;

        LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.001f,
                                                  1000.f);
    }

    std::vector<SceneObject> PhysicsTestingLayer::GetSceneObjects() {
        auto pepeSceneObject = pepe->GetSceneObjects();
        auto groundSceneObjects = ground->GetSceneObjects();
        auto rightWallSceneObjects = rightWall->GetSceneObjects();
        auto leftWallSceneObjects = leftWall->GetSceneObjects();
        auto topWallSceneObjects = topWall->GetSceneObjects();

        std::vector<SceneObject> sceneObjects;

        sceneObjects.insert(sceneObjects.end(), pepeSceneObject.begin(), pepeSceneObject.end());
        sceneObjects.insert(sceneObjects.end(), groundSceneObjects.begin(), groundSceneObjects.end());
        sceneObjects.insert(sceneObjects.end(), rightWallSceneObjects.begin(), rightWallSceneObjects.end());
        sceneObjects.insert(sceneObjects.end(), leftWallSceneObjects.begin(), leftWallSceneObjects.end());
        sceneObjects.insert(sceneObjects.end(), topWallSceneObjects.begin(), topWallSceneObjects.end());

        return sceneObjects;
    }

    void PhysicsTestingLayer::updateViewMatrix() {
        const auto pepePosition = pepe->GetPosition();
        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3{pepePosition.x, pepePosition.y, 3.f}, // Camera position
                                             glm::vec3{pepePosition.x, pepePosition.y, 0.f}, // Target to look at
                                             glm::vec3{0.f, 1.f, 0.f}); // Up vector
    }
}
