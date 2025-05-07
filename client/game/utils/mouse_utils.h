//
// Created by ozzadar on 2025-04-20.
//

#pragma once

#include <glm\glm.hpp>

inline glm::ivec2 CenteredMousePosition(glm::ivec2 pos, glm::ivec2 windowSize) {
  return glm::ivec2 {
      pos.x - windowSize.x / 2,
      (pos.y - windowSize.y / 2) * -1
  };
}