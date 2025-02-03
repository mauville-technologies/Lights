//
// Created by ozzadar on 2025-01-14.
//

#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace OZZ::collision {
    // TODO: If I add more shapes, I might need more information. If I start handling physics, I'll need to add more information
    struct OzzCollisionResult {
        bool bCollided;
        std::vector<glm::vec2> ContactPoints;
        static OzzCollisionResult NoCollision() {
            return {.bCollided = false, .ContactPoints = {}};
        }
    };

    template <typename Collider, typename Collidee>
    OzzCollisionResult IsColliding(const Collider& collider, const Collidee& collidee) {
        return collider.IsColliding(collidee);
    }
}