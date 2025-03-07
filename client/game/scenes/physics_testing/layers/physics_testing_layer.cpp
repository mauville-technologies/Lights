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
        }
    }

    void PhysicsTestingLayer::Init() {
        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), // Camera position
                                             glm::vec3(0.f, 0.f, 0.f), // Target to look at
                                             glm::vec3(0.f, 1.f, 0.f)); // Up vector

        auto [gId, inGround] = gameWorld->CreateGameObject<Sprite>("assets/textures/container.jpg");
        groundId = gId;
        ground = dynamic_cast<Sprite *>(inGround);
        ground->Scale = {12.f * constants::PixelsPerMeter, 1.f * constants::PixelsPerMeter, 1.f};
        ground->Position = {0.f, -2.f * constants::PixelsPerMeter, 1.f};

        auto [rId, inRightWall] = gameWorld->CreateGameObject<Sprite>("assets/textures/container.jpg");
        rightWallId = rId;
        rightWall = dynamic_cast<Sprite *>(inRightWall);
        rightWall->Scale = {1.f * constants::PixelsPerMeter, 12.f * constants::PixelsPerMeter, 1.f};
        rightWall->Position = {6.f * constants::PixelsPerMeter, 0.f, 1.f};

        auto [lId, inLeftWall] = gameWorld->CreateGameObject<Sprite>("assets/textures/container.jpg");
        leftWallId = lId;
        leftWall = dynamic_cast<Sprite *>(inLeftWall);
        leftWall->Scale = {1.f * constants::PixelsPerMeter, 12.f * constants::PixelsPerMeter, 1.f};
        leftWall->Position = {-6.f * constants::PixelsPerMeter, 0.f, 1.f};

        auto [tId, inTopWall] = gameWorld->CreateGameObject<Sprite>("assets/textures/container.jpg");
        topWallId = tId;
        topWall = dynamic_cast<Sprite *>(inTopWall);
        topWall->Scale = {12.f * constants::PixelsPerMeter, 1.f * constants::PixelsPerMeter, 1.f};
        topWall->Position = {0.f, 10.f * constants::PixelsPerMeter, 1.f};

        auto [id, inpepe] = gameWorld->CreateGameObject<Sprite>("assets/textures/pepe.png");
        // scale pepe
        pepeid = id;
        pepe = dynamic_cast<Sprite *>(inpepe);
        pepe->Position = {0, 2.f * constants::PixelsPerMeter, 0.f};
        pepe->Scale = {2.f * constants::PixelsPerMeter, 2.f * constants::PixelsPerMeter, 1.f};

        // add a physics body
        if (auto *pWorld = pepe->GetWorld()) {
            ground->MainBody =
                pWorld->CreateBody(
                    BodyType::Static,
                    OZZ::collision::shapes::OzzRectangle{
                        .Position = ground->Position,
                        .Size = ground->Scale
                    },
                    {0, 0}
                );

            rightWall->MainBody =
                pWorld->CreateBody(
                    BodyType::Static,
                    OZZ::collision::shapes::OzzRectangle{
                        .Position = rightWall->Position,
                        .Size = rightWall->Scale
                    },
                    {0, 0}
                );

            leftWall->MainBody =
                pWorld->CreateBody(
                    BodyType::Static,
                    OZZ::collision::shapes::OzzRectangle{
                        .Position = leftWall->Position,
                        .Size = leftWall->Scale
                    },
                    {0, 0}
                );

            topWall->MainBody =
                    pWorld->CreateBody(
                        BodyType::Static,
                        OZZ::collision::shapes::OzzRectangle{
                            .Position = topWall->Position,
                            .Size = topWall->Scale
                        },
                        {0, 0}
                    );

            pepe->MainBody =
                    pWorld->CreateBody(BodyType::Dynamic,
                                       OZZ::collision::shapes::OzzRectangle
                                       {
                                           .Position = pepe->Position,
                                           .Size = pepe->Scale
                                       },
                                       {0, 0}
                    );
        }

        // Set up jump button
        input->RegisterInputMapping({
            .Action = "Jump",
            .Chord = InputChord{.Keys = std::vector<EKey>{EKey::Space}},
            .Callbacks = {
                .OnPressed = [this]() {
                    if (auto *pWorld = pepe->GetWorld()) {
                        if (auto *body = pWorld->GetBody(pepe->MainBody)) {
                            body->Velocity.y = 20;
                        }
                    }
                },
                .OnReleased = [this]() {
                }
            }
        });


        input->RegisterAxisMapping({
            .Action = "MoveLeftRight",
            .Keys = {
                {EKey::Left, -1},
                {EKey::Right, 1},
                {EKey::A, -1},
                {EKey::D, 1},
            },
        });

        const auto pepebody = pepe->GetWorld()->GetBody(pepe->MainBody);
        auto pepePosition = pepebody->GetPosition();

        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3{pepePosition.x, pepePosition.y, 1.f},
                                             {pepePosition.x, pepePosition.y, 0.f}, {0.f, 1.f, 0.f});
    }

    void PhysicsTestingLayer::PhysicsTick(float DeltaTime) {
        SceneLayer::PhysicsTick(DeltaTime);
    }

    void PhysicsTestingLayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);

        // translate to keep track of pepe
        const auto pepebody = pepe->GetWorld()->GetBody(pepe->MainBody);
        auto pepePosition = pepebody->GetPosition();

        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3{pepePosition.x, pepePosition.y, 1},
                                             {pepePosition.x, pepePosition.y, 0.f}, {0.f, 1.f, 0.f});

        if (auto *pWorld = pepe->GetWorld()) {
            const auto moveValue = input->GetAxisValue("MoveLeftRight");
            if (auto *body = pWorld->GetBody(pepe->MainBody)) {
                body->Velocity.x = moveValue * 10;
            }
        }
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
        // auto pepe2SceneObject = pepe2->GetSceneObjects();

        std::vector<SceneObject> sceneObjects;

        sceneObjects.insert(sceneObjects.end(), pepeSceneObject.begin(), pepeSceneObject.end());
        sceneObjects.insert(sceneObjects.end(), groundSceneObjects.begin(), groundSceneObjects.end());
        sceneObjects.insert(sceneObjects.end(), rightWallSceneObjects.begin(), rightWallSceneObjects.end());
        sceneObjects.insert(sceneObjects.end(), leftWallSceneObjects.begin(), leftWallSceneObjects.end());
        sceneObjects.insert(sceneObjects.end(), topWallSceneObjects.begin(), topWallSceneObjects.end());

        return sceneObjects;
    }
}
