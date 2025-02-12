//
// Created by ozzadar on 2025-01-12.
//

#pragma once
#include <variant>
#include <vector>
#include <random>
#include <optional>
#include <glm/glm.hpp>
#include <thread>

namespace OZZ {
    enum class ShapeKind {
        Unknown,
        Circle,
        Rectangle,
        Polygon,
        Point
    };

    struct CircleDef {
        float Radius{0.f};
    };

    struct RectangleDef {
        glm::vec2 Size{0.f};
    };

    struct PolygonDef {
        std::vector<glm::vec2> Vertices{};
    };

    struct PointDef {
        glm::vec2 Point{0.f};
    };

    enum class BodyType {
        Static,
        Dynamic,
        Kinematic
    };

    using ShapeDefType = std::variant<CircleDef, RectangleDef, PolygonDef, PointDef>;

    struct Body {
        uint64_t ID{};
        BodyType Type{BodyType::Static};
        ShapeKind ShapeType{};
        ShapeDefType Definition{};

        glm::vec2 Position{0.f};
        glm::vec2 Velocity{0.f};

        // TODO: Maybe we want rotation and other junk. For now, let's assume everything stays upright the way the shape is defined
    };

    class OzzWorld2D {
    public:
        uint64_t CreateBody(BodyType type, ShapeKind shapeType, const ShapeDefType &shapeDef, const glm::vec2 &position = {0.f, 0.f},
                            const glm::vec2 &velocity = {0.f, 0.f});
        void DestroyBody(uint64_t id);
        Body* GetBody(uint64_t id);

        void PhysicsTick(float DeltaTime);
    private:
        uint64_t generateUnusedID() {
            static std::random_device rd;
            static std::mt19937_64 gen(rd());
            static std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());

            uint64_t id = dis(gen);

            // We should make sure that the ID is unique
            while (id == 0 || std::any_of(bodies.begin(), bodies.end(), [id](const auto &body) { return body.ID == id; })) {
                id = dis(gen);
            }

            return id;
        }

    private:
        std::mutex bodyMutex{};
        std::vector<Body> bodies{};
    };
} // OZZ
