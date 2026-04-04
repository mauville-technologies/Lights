//
// Created by Claude on 2026-04-04.
//

#include "ozz_collision/raycast/ozz_plane3d.h"

namespace OZZ::collision::raycast {

    OzzPlane3D OzzPlane3D::FromPositionAndOrientation(const glm::vec3& position, const glm::quat& orientation) {
        // Default frame: shape lies flat on XZ plane (Y-up)
        // Right = +X, Up = -Z (so that local +Y maps to world -Z), Normal = +Y
        const glm::vec3 right = orientation * glm::vec3{1.f, 0.f, 0.f};
        const glm::vec3 up = orientation * glm::vec3{0.f, 0.f, -1.f};
        const glm::vec3 normal = orientation * glm::vec3{0.f, 1.f, 0.f};

        return {
            .Origin = position,
            .Normal = normal,
            .Right = right,
            .Up = up,
        };
    }

    OzzPlane3D OzzPlane3D::FromModelMatrix(const glm::mat4& model) {
        // Extract and normalize the axis columns from the model matrix
        const glm::vec3 right = glm::normalize(glm::vec3{model[0]});
        const glm::vec3 up = glm::normalize(glm::vec3{model[1]});
        const glm::vec3 normal = glm::normalize(glm::cross(right, up));
        const glm::vec3 origin = glm::vec3{model[3]};

        return {
            .Origin = origin,
            .Normal = normal,
            .Right = right,
            .Up = up,
        };
    }

} // namespace OZZ::collision::raycast
