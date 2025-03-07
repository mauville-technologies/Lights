//
// Created by ozzadar on 2025-01-12.
//

#pragma once
#include <variant>
#include <vector>
#include <random>
#include <limits>
#include <glm/glm.hpp>
#include <mutex>
#include <ozz_collision/ozz_collision_shapes.h>

namespace OZZ {
    using namespace OZZ::collision::shapes;

    using BodyID = uint64_t;
    constexpr auto InvalidBodyID = std::numeric_limits<BodyID>::max();

    enum class BodyType {
        Static,
        Dynamic,
        Kinematic
    };

    struct Body {
        BodyID ID{};
        BodyType Type{BodyType::Static};
        OzzShapeData Data{};

        [[nodiscard]] glm::vec3 GetPosition() const {
            return std::visit([](const auto& shape) {
                return glm::vec3{shape.Position, 1.f};
            }, Data);
        }

        void SetPosition(const glm::vec2& position) {
            std::visit([position](auto& shape) {
                shape.Position = position;
            }, Data);
        }

        glm::vec2 GetScale() const {
            return std::visit([](auto& shape) {
                return shape.Scale();
            }, Data);
        }
        glm::vec2 Velocity{0.f};

        [[nodiscard]] OzzShapeKind Kind() const {
            return static_cast<OzzShapeKind>(Data.index());
        }

        // TODO: Maybe we want rotation and other junk. For now, let's assume everything stays upright the way the shape is defined
        bool bCollidedThisFrame{false};
    };


    namespace collision {
        struct CollisionVisitor {
            template <typename A, typename B>
            collision::OzzCollisionResult operator()(const A& a, const B& b) const {
                return IsColliding(a, b);
            }
        };

        static collision::OzzCollisionResult IsColliding(const Body& a, const Body& b) {
            return std::visit(CollisionVisitor{}, a.Data, b.Data);
        }
    }


    class OzzWorld2D {
    public:
        BodyID CreateBody(BodyType type, const OzzShapeData& shapeDef,
                          const glm::vec2& velocity = {0.f, 0.f});
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
            while (id == 0 || std::any_of(bodies.begin(), bodies.end(), [id](const auto& body) {
                return body.ID == id;
            })) {
                id = dis(gen);
            }

            return id;
        }

    private:
        std::mutex bodyMutex{};
        std::vector<Body> bodies{};
    };
} // OZZ
