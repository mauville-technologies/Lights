//
// Created by ozzadar on 2025-01-09.
//

#pragma once
#include <glm/glm.hpp>
#include "constants.h"

namespace OZZ::units {
    // OZZ
    inline glm::vec2 PixelToMeters(const glm::vec2 &inVector) {
        return glm::vec2{
            inVector.x / OZZ::game::constants::PixelsPerMeter,
            -inVector.y / OZZ::game::constants::PixelsPerMeter
        };
    }

    inline glm::vec2 MetersToPixels(const glm::vec2 &inVector) {
        return glm::vec2{
            inVector.x * OZZ::game::constants::PixelsPerMeter,
            -inVector.y * OZZ::game::constants::PixelsPerMeter
        };
    }

    inline glm::vec2 PhysicsUnitToMeters(const glm::vec2 &inVector) {
        return {
            inVector.x / OZZ::game::constants::PhysicsUnitPerMeter,
            inVector.y / OZZ::game::constants::PhysicsUnitPerMeter
        };
    }

    inline glm::vec2 MetersToPhysicsUnit(const glm::vec2 &inVector) {
        return {
            inVector.x * OZZ::game::constants::PhysicsUnitPerMeter,
            inVector.y * OZZ::game::constants::PhysicsUnitPerMeter
        };
    }

    inline glm::vec2 PixelsToPhysics(const glm::vec2 &inVector) {
        return MetersToPhysicsUnit(PixelToMeters(inVector));
    }

    inline glm::vec2 PhysicsUnitToPixels(const glm::vec2 &inVector) {
        return MetersToPixels(PhysicsUnitToMeters(inVector));
    }
}
