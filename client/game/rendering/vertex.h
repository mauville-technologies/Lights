//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace OZZ {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 normal;
        glm::vec2 texCoords;

        inline static void BindAttribPointers() {
            // Position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
            // Color
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
            // Normal
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
            // TexCoords
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        }
    };
} // OZZ
