//
// Created by ozzadar on 2025-04-20.
//

#pragma once

#include <glm/glm.hpp>

/**
 *
 * @param pos Position in window coordinates (top-left is (0,0))
 * @param windowSize Size of the window
 * @return Position centered around (0,0) (center is (0,0), top-left is (-width/2, height/2))
 */
inline glm::ivec2 CenteredMousePosition(glm::ivec2 pos, glm::ivec2 windowSize) {
    return glm::ivec2{pos.x - windowSize.x / 2, (pos.y - windowSize.y / 2) * -1};
}

/**
 *
 * @param screenPos Position in window coordinates (top-left is (0,0))
 * @param windowSize size of the window
 * @param projectionMatrix projection matrix of the camera
 * @param viewMatrix view matrix of the camera
 * @return
 */
inline glm::vec2 ScreenToWorldPosition(glm::ivec2 screenPos,
                                       glm::ivec2 windowSize,
                                       const glm::mat4& projectionMatrix,
                                       const glm::mat4& viewMatrix) {
    // First convert to centered coordinates
    const glm::vec2 centeredPos = CenteredMousePosition(screenPos, windowSize);

    // Convert to normalized device coordinates (NDC)
    const glm::vec4 clipSpacePos =
        glm::vec4(centeredPos.x / (windowSize.x / 2.0f), centeredPos.y / (windowSize.y / 2.0f), 0.0f, 1.0f);

    // Convert to world space
    const glm::mat4 inverseViewProj = glm::inverse(projectionMatrix * viewMatrix);
    glm::vec4 worldSpacePos = inverseViewProj * clipSpacePos;

    // Perspective divide
    worldSpacePos /= worldSpacePos.w;

    return {worldSpacePos.x, worldSpacePos.y};
}