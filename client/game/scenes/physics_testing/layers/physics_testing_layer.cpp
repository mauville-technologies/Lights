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
            world->RemoveObject(pepe2id);
        }
    }

    void PhysicsTestingLayer::Init() {
        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), // Camera position
                                             glm::vec3(0.f, 0.f, 0.f), // Target to look at
                                             glm::vec3(0.f, 1.f, 0.f)); // Up vector

        auto [id, inpepe] = world->CreateGameObject<Sprite>("assets/textures/pepe.png");
        // scale pepe
        pepeid = id;
        pepe = static_cast<Sprite*>(inpepe);
        pepe->Scale = {2.f, 2.f, 1.f};

        // add a physics body
        if (auto* pWorld = pepe->GetWorld()) {
            // we'll simply make a square at (0,0)
            // pepe->Bodies.emplace_back(pWorld->CreateBody(BodyType::Dynamic, OzzShapeKind::Circle,
            //                    OZZ::collision::shapes::OzzCircle
            //                    {
            //                        .Position = pepe->Position,
            //                        .Radius = pepe->Scale.x
            //                    },
            //                    pepe->Position,
            //                    {0, 10}
            // ));

            //  pepe->Bodies.emplace_back(pWorld->CreateBody(BodyType::Dynamic, OzzShapeKind::Rectangle,
            //                    OZZ::collision::shapes::OzzRectangle
            //                    {
            //                        .Position = pepe->Position,
            //                        .Size = pepe->Scale
            //                    },
            //                    {0, 0},
            //                    {0, 25}
            // ));
            pepe->Bodies.emplace_back(pWorld->CreateBody(BodyType::Dynamic, OzzShapeKind::Point,
                                                         OZZ::collision::shapes::OzzPoint
                                                         {
                                                             .Position = pepe->Position,
                                                         },
                                                         {0, 0},
                                                         {0, 5}
            ));
        }
    }

    void PhysicsTestingLayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);
        if (pepe && !pepe->Bodies.empty()) {
            auto* body = pepe->GetWorld()->GetBody(pepe->Bodies[0]);
            auto position = GetOzzShapePosition(body->Kind, body->Data);
            pepe->Position = position;
        }
    }

    void PhysicsTestingLayer::RenderTargetResized(glm::ivec2 size) {
        spdlog::info("Physics Resized to: {}x{}", size.x, size.y);
        const auto width = size.x;
        const auto height = size.y;

        LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.1f, 100.f);
    }

    std::vector<SceneObject> PhysicsTestingLayer::GetSceneObjects() {
        auto pepeSceneObject = pepe->GetSceneObjects();
        // auto pepe2SceneObject = pepe2->GetSceneObjects();

        std::vector<SceneObject> sceneObjects;
        // sceneObjects.reserve(pepeSceneObject.size() + pepe2SceneObject.size());
        sceneObjects.insert(sceneObjects.end(), pepeSceneObject.begin(), pepeSceneObject.end());
        // sceneObjects.insert(sceneObjects.end(), pepe2SceneObject.begin(), pepe2SceneObject.end());
        return sceneObjects;
    }
}
