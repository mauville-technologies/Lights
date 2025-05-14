//
// Created by ozzadar on 2025-04-20.
//

#pragma once

inline glm::ivec2 CenteredMousePosition(glm::ivec2 pos, glm::ivec2 windowSize) {
  return glm::ivec2 {
      pos.x - windowSize.x / 2,
      (pos.y - windowSize.y / 2) * -1
  };
}

inline glm::vec2 ScreenToWorldPosition(glm::ivec2 screenPos, glm::ivec2 windowSize, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    // First convert to centered coordinates
    glm::vec2 centeredPos = CenteredMousePosition(screenPos, windowSize);
    
    // Convert to normalized device coordinates (NDC)
    glm::vec4 clipSpacePos = glm::vec4(centeredPos.x / (windowSize.x / 2.0f), 
                                      centeredPos.y / (windowSize.y / 2.0f), 
                                      0.0f, 1.0f);
    
    // Convert to world space
    glm::mat4 inverseViewProj = glm::inverse(projectionMatrix * viewMatrix);
    glm::vec4 worldSpacePos = inverseViewProj * clipSpacePos;
    
    // Perspective divide
    worldSpacePos /= worldSpacePos.w;
    
    return glm::vec2(worldSpacePos.x, worldSpacePos.y);
}