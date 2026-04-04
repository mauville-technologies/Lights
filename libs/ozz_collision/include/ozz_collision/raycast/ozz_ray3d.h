//
// Created by Claude on 2026-04-04.
//

#pragma once

#include <glm/glm.hpp>

namespace OZZ::collision::raycast {
    struct OzzRay3D {
        glm::vec3 Origin;
        glm::vec3 Direction; // must be normalized
    };
} // namespace OZZ::collision::raycast
