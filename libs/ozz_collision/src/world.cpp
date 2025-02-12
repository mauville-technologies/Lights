//
// Created by ozzadar on 2025-01-12.
//

#include "ozz_collision/world.h"

namespace OZZ {
    uint64_t OzzWorld2D::CreateBody(BodyType type, ShapeKind shapeType, const ShapeDefType& shapeDef,
                                    const glm::vec2& position,
                                    const glm::vec2& velocity) {
        const auto newId = generateUnusedID();

        {
            auto lock = std::lock_guard{bodyMutex};
            bodies.emplace_back(Body{
                .ID = newId,
                .Type = type,
                .Position = position,
                .ShapeType = shapeType,
                .Definition = shapeDef,
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
        // we should probably add a mutex around accessing the bodies vector
        // that way we can ensure that physicstick is always called after all the bodies have been added/removed
        // Ticking world
        for (auto& body : bodies) {
            // Apply forces

            switch (body.Type) {
            case BodyType::Dynamic:
                // Apply gravity
                body.Velocity.y -= 9.81f * DeltaTime;
                // Apply velocity
                body.Position += body.Velocity * DeltaTime;
                break;
            case BodyType::Kinematic:
                break;
            case BodyType::Static:
                break;
            default:
                break;
            }
        }
    }
} // OZZ
