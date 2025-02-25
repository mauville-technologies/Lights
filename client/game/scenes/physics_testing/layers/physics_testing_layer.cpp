//
// Created by ozzadar on 2025-02-12.
//

#include "physics_testing_layer.h"
#include "lights/game/game_object.h"

namespace OZZ::game::scene {
    PhysicsTestingLayer::PhysicsTestingLayer(GameWorld* inWorld) : world(inWorld) {}

    PhysicsTestingLayer::~PhysicsTestingLayer() {
        if (world) {
            world->RemoveObject(pepeid);
        }
    }

    void PhysicsTestingLayer::Init() {
        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), // Camera position
                                             glm::vec3(0.f, 0.f, 0.f), // Target to look at
                                             glm::vec3(0.f, 1.f, 0.f)); // Up vector

        auto [ gId, inGround ] = world->CreateGameObject<Sprite>("assets/textures/container.jpg");
        groundId = gId;
        ground = dynamic_cast<Sprite*>(inGround);
        ground->Scale = {12.f * constants::PixelsPerMeter, 1.f * constants::PixelsPerMeter, 1.f};
        ground->Position = { 0.f, -2.f * constants::PixelsPerMeter, 1.f};

        auto [id, inpepe] = world->CreateGameObject<Sprite>("assets/textures/pepe.png");
        // scale pepe
        pepeid = id;
        pepe = dynamic_cast<Sprite*>(inpepe);
        pepe->Position = {0, 2.f * constants::PixelsPerMeter, 0.f};
        pepe->Scale = {2.f * constants::PixelsPerMeter, 2.f * constants::PixelsPerMeter, 1.f};

        // add a physics body
        if (auto* pWorld = pepe->GetWorld()) {
            ground->MainBody = ground->Bodies.emplace_back(
                pWorld->CreateBody(
                    BodyType::Static,
                    OzzShapeKind::Rectangle,
                    OZZ::collision::shapes::OzzRectangle {
                        .Position = ground->Position,
                        .Size = ground->Scale
                    },
                    ground->Position,
                    {0, 0 }
                )
            );
            // we'll simply make a square at (0,0)
            pepe->MainBody = pepe->Bodies.emplace_back(pWorld->CreateBody(BodyType::Dynamic, OzzShapeKind::Circle,
                               OZZ::collision::shapes::OzzCircle
                               {
                                   .Position = pepe->Position,
                                   .Radius = pepe->Scale.x / 2
                               },
                               pepe->Position,
                               {0, 10}
            ));
            //
            //  pepe->MainBody = pepe->Bodies.emplace_back(pWorld->CreateBody(BodyType::Dynamic, OzzShapeKind::Rectangle,
            //                    OZZ::collision::shapes::OzzRectangle
            //                    {
            //                        .Position = pepe->Position,
            //                        .Size = pepe->Scale
            //                    },
            //                    {0, 0},
            //                    {0, 2}
            // ));
            // pepe->Bodies.emplace_back(pWorld->CreateBody(BodyType::Dynamic, OzzShapeKind::Point,
            //                                              OZZ::collision::shapes::OzzPoint
            //                                              {
            //                                                  .Position = pepe->Position,
            //                                              },
            //                                              {0, 0},
            //                                              {0, 5}
            // ));
        }
    }

    void PhysicsTestingLayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);
    }

    void PhysicsTestingLayer::RenderTargetResized(glm::ivec2 size) {
        spdlog::info("Physics Resized to: {}x{}", size.x, size.y);
        const auto width = size.x;
        const auto height = size.y;

        LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.1f, 100.f);
    }

    std::vector<SceneObject> PhysicsTestingLayer::GetSceneObjects() {
        auto pepeSceneObject = pepe->GetSceneObjects();
        auto groundSceneObjects = ground->GetSceneObjects();
        // auto pepe2SceneObject = pepe2->GetSceneObjects();

        std::vector<SceneObject> sceneObjects;
        // sceneObjects.reserve(pepeSceneObject.size() + pepe2SceneObject.size());
        sceneObjects.insert(sceneObjects.end(), pepeSceneObject.begin(), pepeSceneObject.end());
        sceneObjects.insert(sceneObjects.end(), groundSceneObjects.begin(), groundSceneObjects.end());
        // sceneObjects.insert(sceneObjects.end(), pepe2SceneObject.begin(), pepe2SceneObject.end());
        return sceneObjects;
    }
}
