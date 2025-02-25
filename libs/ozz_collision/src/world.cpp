//
// Created by ozzadar on 2025-01-12.
//

#include "ozz_collision/world.h"

#include "ozz_collision/ozz_collision_shapes.h"
#include <iostream>

namespace OZZ {
    uint64_t OzzWorld2D::CreateBody(BodyType type, OzzShapeKind shapeType, const OzzShapeData& shapeDef,
                                    const glm::vec2& position,
                                    const glm::vec2& velocity) {
        const auto newId = generateUnusedID();

        {
            auto lock = std::lock_guard{bodyMutex};
            bodies.emplace_back(Body{
                .ID = newId,
                .Type = type,
                .Kind = shapeType,
                .Data = shapeDef,
                .Position = position,
                .Velocity = velocity,
            });
        }
        return newId;
    }

    void OzzWorld2D::DestroyBody(uint64_t id) {
        auto lock = std::lock_guard{bodyMutex};
        std::erase_if(bodies, [id](const Body& body) {
            return body.ID == id;
        });
    }

    Body* OzzWorld2D::GetBody(uint64_t id) {
        auto lock = std::lock_guard{bodyMutex};
        const auto body = std::find_if(bodies.begin(), bodies.end(), [id](const Body& body) {
            return body.ID == id;
        });

        if (body != bodies.end()) {
            return &*body;
        }

        return nullptr;
    }

    void OzzWorld2D::PhysicsTick(float DeltaTime) {
        std::vector<Body*> dynamicBodies{};
        std::vector<Body*> staticBodies{};
        std::vector<Body*> kinematicBodies{};
        // we should probably add a mutex around accessing the bodies vector
        // that way we can ensure that physicstick is always called after all the bodies have been added/removed
        // Ticking world
        for (auto& body : bodies) {
            // Apply forces
            switch (body.Type) {
            case BodyType::Dynamic: {
                // Apply gravity
                body.Velocity.y -= 20.f * DeltaTime;
                // Apply velocity

                auto position = GetOzzShapePosition(body.Kind, body.Data);
                position += glm::vec3{body.Velocity, 1.f};
                SetOzzShapePosition(body.Kind, body.Data, position);
                dynamicBodies.emplace_back(&body);
                break;
            }
            case BodyType::Kinematic:
                kinematicBodies.emplace_back(&body);
                break;
            case BodyType::Static:
                staticBodies.emplace_back(&body);
                break;
            default:
                break;
            }
        }

        // Collision detection
        for (auto* dynamicBody : dynamicBodies) {
            for (auto* staticBody : staticBodies) {
                // check if they collide
                auto bColliding = collision::IsColliding(*dynamicBody, *staticBody);
                dynamicBody->bCollidedThisFrame = bColliding.bCollided;
                staticBody->bCollidedThisFrame = bColliding.bCollided;
            }
        }
    }
} // OZZ
