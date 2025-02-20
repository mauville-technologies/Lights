//
// Created by ozzadar on 2025-01-12.
//

#pragma once
#include <variant>
#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <mutex>
#include <ozz_collision/ozz_collision_shapes.h>

namespace OZZ {
    using namespace OZZ::collision::shapes;

    using BodyID = uint64_t;

    enum class BodyType {
        Static,
        Dynamic,
        Kinematic
    };

    struct Body {
        BodyID ID{};
        BodyType Type{BodyType::Static};
        OzzShapeKind Kind{};
        OzzShapeData Data{};

        // TODO: I feel like this position variable is conflicting with the shape definitions.
        // at the very least I'll probably need a way to pass in a translation when I'm doing the actual collision detection
        glm::vec2 Position{0.f};
        glm::vec2 Velocity{0.f};

        // TODO: Maybe we want rotation and other junk. For now, let's assume everything stays upright the way the shape is defined
    };

    class OzzWorld2D {
    public:
        BodyID CreateBody(BodyType type, OzzShapeKind shapeType, const OzzShapeData &shapeDef, const glm::vec2 &position = {0.f, 0.f},
                            const glm::vec2 &velocity = {0.f, 0.f});
        void DestroyBody(BodyID id);
        Body* GetBody(BodyID id);

        void PhysicsTick(float DeltaTime);
    private:
        BodyID generateUnusedID() {
            static std::random_device rd;
            static std::mt19937_64 gen(rd());
            static std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());

            BodyID id = dis(gen);

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
