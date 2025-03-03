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
            body.bCollidedThisFrame = false;
            // Apply forces
            switch (body.Type) {
            case BodyType::Dynamic: {
                // Apply gravity
                body.Velocity.y -= 50.f * DeltaTime;
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
        std::vector<std::tuple<Body*, Body*, collision::OzzCollisionResult>> collisions{};
        for (auto* dynamicBody : dynamicBodies) {
            for (auto* staticBody : staticBodies) {
                // check if they collide
                auto collisionResult = collision::IsColliding(*dynamicBody, *staticBody);
                dynamicBody->bCollidedThisFrame = dynamicBody->bCollidedThisFrame || collisionResult.bCollided;
                staticBody->bCollidedThisFrame = staticBody->bCollidedThisFrame || collisionResult.bCollided;
                if (collisionResult.bCollided) {
                    collisions.emplace_back(dynamicBody, staticBody, collisionResult);
                }
            }

            for (auto* kinematicBody : kinematicBodies) {
                // check if they collide
                auto collisionResult = collision::IsColliding(*dynamicBody, *kinematicBody);
                dynamicBody->bCollidedThisFrame = dynamicBody->bCollidedThisFrame || collisionResult.bCollided;
                kinematicBody->bCollidedThisFrame = kinematicBody->bCollidedThisFrame || collisionResult.bCollided;
                if (collisionResult.bCollided) {
                    collisions.emplace_back(dynamicBody, kinematicBody, collisionResult);
                }
            }

            for (auto* otherDynamicBody : dynamicBodies) {
                if (dynamicBody == otherDynamicBody) {
                    continue;
                }
                // check if they collide
                auto collisionResult = collision::IsColliding(*dynamicBody, *otherDynamicBody);
                dynamicBody->bCollidedThisFrame = dynamicBody->bCollidedThisFrame || collisionResult.bCollided;
                otherDynamicBody->bCollidedThisFrame = otherDynamicBody->bCollidedThisFrame || collisionResult.bCollided;
                if (collisionResult.bCollided) {
                    collisions.emplace_back(dynamicBody, otherDynamicBody, collisionResult);
                }
            }
        }

        // resolve collisions
        for (auto& [my, other, collisionResult] : collisions) {
            if (my->Type == BodyType::Dynamic) {
                // push away by normal
                auto position = GetOzzShapePosition(my->Kind, my->Data);
                position += glm::vec3{collisionResult.CollisionNormal * 2.f, 0.f} * 0.1f;
                SetOzzShapePosition(my->Kind, my->Data, position);

                // if normal is pointig up or down, cancel out y
                if (collisionResult.CollisionNormal.y != 0) {
                    // if velocity is different signedness as normal, cancel out y
                    if (std::signbit(my->Velocity.y) != std::signbit(collisionResult.CollisionNormal.y)) {
                        my->Velocity.y = 0;
                    }
                }

                // if the normal is pointing left or right, cancel out x
                if (collisionResult.CollisionNormal.x != 0) {
                    // if velocity is different signedness as normal, cancel out x
                    if (std::signbit(my->Velocity.x) != std::signbit(collisionResult.CollisionNormal.x)) {
                        my->Velocity.x = 0;
                    }
                }
            }
        }
    }
} // OZZ
