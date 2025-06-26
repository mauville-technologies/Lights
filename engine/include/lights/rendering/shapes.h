//
// Created by ozzadar on 2024-12-19.
//

#pragma once

#include <spdlog/spdlog.h>

#include <array>
#include <glm/glm.hpp>
#include <numbers>
#include <utility>

#include "vertex.h"

namespace OZZ {
    constexpr double PI = std::numbers::pi;

    inline glm::vec3 getNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
                               bool print = false) {
        glm::vec3 U = p2 - p1;
        glm::vec3 V = p3 - p1;

        auto normal = glm::cross(U, V);
        //        glm::vec3 normal {
        //            (U.y * V.z) - (U.z * V.y),
        //            (U.z * V.x) - (U.x * V.z),
        //            (U.x * V.y) - (U.y * V.x)
        //        };

        if (print) {
            spdlog::info("Normal: ({},{},{})", normal.x, normal.y, normal.z);
        }

        return normal;
    }

    constexpr std::array<uint32_t, 6> quadIndices{0, 1, 3, 1, 2, 3};
    constexpr std::array<uint32_t, 8> quadOutlineIndices{0, 1, 1, 2, 2, 3, 3, 0};

    constexpr std::array<Vertex, 4> quadVertices{
        {
            {
                .position = {0.5f, 0.5f, 0.0f},
                .normal = {0, 0, 1.f},
                .uv = {1.0f, 1.0f}
            },
            {
                .position = {0.5f, -0.5f, 0.0f},
                .normal = {0, 0, 1.f},
                .uv = {1.0f, 0.0f}
            },
            {
                .position = {-0.5f, -0.5f, 0.0f},
                .normal = {0, 0, 1.f},
                .uv = {0.0f, 0.0f}
            },
            {
                .position = {-0.5f, 0.5f, 0.0f},
                .normal = {0, 0, 1.f},
                .uv = {0.0f, 1.0f}
            }
        }
    };
    constexpr std::array<uint32_t, 36> cubeIndices{
        // Front
        0,
        1,
        3,
        1,
        2,
        3,
        // Back
        4,
        5,
        7,
        5,
        6,
        7,
        // Left
        8,
        9,
        11,
        9,
        10,
        11,
        // Right
        12,
        13,
        15,
        13,
        14,
        15,
        // Top
        16,
        17,
        19,
        17,
        18,
        19,
        // Bottom
        20,
        21,
        23,
        21,
        22,
        23,
    };

    constexpr std::array<uint32_t, 36> invertedCubeIndices{
        // Front
        3,
        1,
        0,
        3,
        2,
        1,
        // Back
        7,
        5,
        4,
        7,
        6,
        5,
        // Left
        11,
        9,
        8,
        11,
        10,
        9,
        // Right
        15,
        13,
        12,
        15,
        14,
        13,
        // Top
        19,
        17,
        16,
        19,
        18,
        17,
        // Bottom
        23,
        21,
        20,
        23,
        22,
        21,
    };

    constexpr std::array<Vertex, 24> cubeVertices{
        // FRONT
        Vertex{
            .position = {0.5f, 0.5f, 0.5f},
            .normal = {0, 0, 1.f},
            .uv = {1.f, 1.f},
        },
        {
            .position = {0.5f, -0.5f, 0.5f},
            .normal = {0, 0, 1.f},
            .uv = {1.f, 0.f},
        },
        {
            .position = {-0.5f, -0.5f, 0.5f},
            .normal = {0, 0, 1.f},
            .uv = {0.f, 0.f},
        },
        {
            .position = {-0.5f, 0.5f, 0.5f},
            .normal = {0, 0, 1.f},
            .uv = {0.f, 1.f},
        },

        // BACK
        {
            .position = {-0.5f, 0.5f, -0.5f},
            .normal = {0, 0, -1.f},
            .uv = {1.f, 1.f},
        },
        {
            .position = {-0.5f, -0.5f, -0.5f},
            .normal = {0, 0, -1.f},
            .uv = {1.f, 0.f},
        },
        {
            .position = {0.5f, -0.5f, -0.5f},
            .normal = {0, 0, -1.f},
            .uv = {0.f, 0.f},
        },
        {
            .position = {0.5f, 0.5f, -0.5f},
            .normal = {0, 0, -1.f},
            .uv = {0.f, 1.f},
        },

        // LEFT
        {
            .position = {-0.5f, 0.5f, 0.5f},
            .normal = {-1.f, 0, 0.f},
            .uv = {1.f, 1.f},
        },
        {
            .position = {-0.5f, -0.5f, 0.5f},
            .normal = {-1.f, 0, 0.f},
            .uv = {1.f, 0.f},
        },
        {
            .position = {-0.5f, -0.5f, -0.5f},
            .normal = {-1.f, 0, 0.f},
            .uv = {0.f, 0.f},
        },
        {
            .position = {-0.5f, 0.5f, -0.5f},
            .normal = {-1.f, 0, 0.f},
            .uv = {0.f, 1.f},
        },

        // RIGHT
        {
            .position = {0.5f, 0.5f, -0.5f},
            .normal = {1.f, 0, 0.f},
            .uv = {1.f, 1.f},
        },
        {
            .position = {0.5f, -0.5f, -0.5f},
            .normal = {1.f, 0, 0.f},
            .uv = {1.f, 0.f},
        },

        {
            .position = {0.5f, -0.5f, 0.5f},
            .normal = {1.f, 0, 0.f},
            .uv = {0.f, 0.f},
        },
        {
            .position = {0.5f, 0.5f, 0.5f},
            .normal = {1.f, 0, 0.f},
            .uv = {0.f, 1.f},
        },

        // TOP
        {
            .position = {0.5f, 0.5f, -0.5f},
            .normal = {0.f, 1.f, 0.f},
            .uv = {1.f, 1.f},
        },
        {
            .position = {0.5f, 0.5f, 0.5f},
            .normal = {0.f, 1.f, 0.f},
            .uv = {1.f, 0.f},
        },

        {
            .position = {-0.5f, 0.5f, 0.5f},
            .normal = {0.f, 1.f, 0.f},
            .uv = {0.f, 0.f},
        },
        {
            .position = {-0.5f, 0.5f, -0.5f},
            .normal = {0.f, 1.f, 0.f},
            .uv = {0.f, 1.f},
        },

        // BOTTOM
        {
            .position = {0.5f, -0.5f, 0.5f},
            .normal = {0.f, -1.f, 0.f},
            .uv = {1.f, 1.f},
        },
        {
            .position = {0.5f, -0.5f, -0.5f},
            .normal = {0.f, -1.f, 0.f},
            .uv = {1.f, 0.f},
        },

        {
            .position = {-0.5f, -0.5f, -0.5f},
            .normal = {0.f, -1.f, 0.f},
            .uv = {0.f, 0.f},
        },
        {
            .position = {-0.5f, -0.5f, 0.5f},
            .normal = {0.f, -1.f, 0.f},
            .uv = {0.f, 1.f},
        },
    };

    constexpr std::array<uint32_t, 18> pyramidIndices{
        // Front Tri
        0,
        1,
        2,
        // Right Tri
        3,
        4,
        5,
        // Back Tri
        6,
        7,
        8,
        // Left Tri
        9,
        10,
        11,
        // Bottom quad
        12,
        13,
        14,
        13,
        15,
        14,
    };

    constexpr std::array<Vertex, 16> pyramidVertices{
        // FRONT TRI
        Vertex{
            .position = {-0.5f, 0.f, 0.5f},
            .normal = {0, 0.5, 1.f},
            .uv = {0.f, 0.f},
        },
        {
            .position = {0.f, 1.f, 0.f},
            .normal = {0, 0.5, 1.f},
            .uv = {0.5f, 1.f},
        },
        {
            .position = {0.5f, 0.f, 0.5f},
            .normal = {0, 0.5, 1.f},
            .uv = {1.f, 0.f},
        },
        // RIGHT TRI
        {
            .position = {0.5f, 0.f, 0.5f},
            .normal = {1.f, 0.5f, 0.f},
            .uv = {0.f, 0.f},
        },
        {
            .position = {0.f, 1.f, 0.f},
            .normal = {1.f, 0.5f, 0.f},
            .uv = {0.5f, 1.f},
        },
        {
            .position = {0.5f, 0.f, -0.5f},
            .normal = {1.f, 0.5f, 0.f},
            .uv = {1.f, 0.f},
        },
        // BACK TRI
        {
            .position = {0.5f, 0.f, -0.5f},
            .normal = {0, 0.5, -1.f},
            .uv = {0.f, 0.f},
        },
        {
            .position = {0.f, 1.f, 0.f},
            .normal = {0, 0.5, -1.f},
            .uv = {0.5f, 1.f},
        },
        {
            .position = {-0.5f, 0.f, -0.5f},
            .normal = {0, 0.5, -1.f},
            .uv = {1.f, 0.f},
        },
        // LEFT TRI
        {
            .position = {-0.5f, 0.f, -0.5f},
            .normal = {-1.f, 0.5f, 0.f},
            .uv = {0.f, 0.f},
        },
        {
            .position = {0.f, 1.f, 0.f},
            .normal = {-1.f, 0.5f, 0.f},
            .uv = {0.5f, 1.f},
        },
        {
            .position = {-0.5f, 0.f, 0.5f},
            .normal = {-1.f, 0.5f, 0.f},
            .uv = {1.f, 0.f},
        },
        // Bottom square
        {
            .position = {-0.5f, 0.f, -0.5f},
            .normal = {0, -1.f, 0},
            .uv = {0.f, 0.f},
        },
        {
            .position = {-0.5f, 0.f, 0.5f},
            .normal = {0, -1.f, 0},
            .uv = {0.f, 1.f},
        },
        {
            .position = {0.5f, 0.f, -0.5f},
            .normal = {0, -1.f, 0},
            .uv = {1.f, 0.f},
        },
        {
            .position = {0.5f, 0.f, 0.5f},
            .normal = {0, -1.f, 0},
            .uv = {1.f, 1.f},
        }
    };

    static std::pair<std::vector<Vertex>, std::vector<uint32_t> > GenerateCircle(
        float radius = 1.f, uint32_t sectors = 50, bool bOutlineOnly = false) {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        const double sectorStep = 2 * PI / sectors;

        for (auto i = 0; i <= sectors; ++i) {
            const double sectorAngle = i * sectorStep;
            float x = radius * cosf(static_cast<float>(sectorAngle));
            float y = radius * sinf(static_cast<float>(sectorAngle));

            vertices.push_back(Vertex{
                .position = {x, y, 0.f},
                .normal = {0, 0, 1.f},
                .uv = {
                    0.5f + 0.5f * cosf(static_cast<float>(sectorAngle)),
                    0.5f + 0.5f * sinf(static_cast<float>(sectorAngle))
                },
            });

            if (i > 0) {
                if (!bOutlineOnly) {
                    // if we're doing the whole thing, indices are triangles not lines
                    indices.push_back(0);
                }
                indices.push_back(i);
                indices.push_back(i - 1);
            }
        }

        return {vertices, indices};
    }

    static std::pair<std::vector<Vertex>, std::vector<uint32_t> > GenerateSphere(
        float radius = 1.f, uint32_t sectors = 50, uint32_t stacks = 50) {
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};
        uint32_t k1, k2;
        float x, y, z, xy; // vertex position
        float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal
        float s, t; // vertex texCoord

        double sectorStep = 2 * PI / sectors;
        double stackStep = PI / stacks;
        double sectorAngle, stackAngle;

        for (uint32_t i = 0; i <= stacks; ++i) {
            stackAngle = PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
            xy = radius * cosf(static_cast<float>(stackAngle)); // r * cos(u)
            z = radius * sinf(static_cast<float>(stackAngle)); // r * sin(u)

            k1 = i * (sectors + 1); // beginning of current stack
            k2 = k1 + sectors + 1;
            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal,
            // but different tex coords
            for (uint32_t j = 0; j <= sectors; ++j, ++k1, ++k2) {
                sectorAngle =
                        static_cast<float>(j) * sectorStep; // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy *
                    cosf(static_cast<float>(sectorAngle)); // r * cos(u) * cos(v)
                y = xy *
                    sinf(static_cast<float>(sectorAngle)); // r * cos(u) * sin(v)

                // normalized vertex normal (nx, ny, nz)
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;

                // vertex tex coord (s, t) range between [0, 1]
                s = (float) j / static_cast<float>(sectors);
                t = (float) i / static_cast<float>(stacks);

                vertices.push_back(Vertex{
                    .position = {x, y, z},
                    .normal = {nx, ny, nz},
                    .uv = {s, t},
                });
            }
        }

        for (uint32_t i = 0; i < stacks; ++i) {
            k1 = i * (sectors + 1); // beginning of current stack
            k2 = k1 + sectors + 1; // beginning of next stack

            for (uint32_t j = 0; j < sectors; ++j, ++k1, ++k2) {
                // 2 triangles per sector excluding first and last
                // stacks k1 => k2 => k1+1
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                }

                // k1+1 => k2 => k2+1
                if (i != (stacks - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2 + 1);
                    indices.push_back(k2);
                }
            }
        }

        return {vertices, indices};
    }
} // namespace OZZ
