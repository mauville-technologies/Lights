//
// Created by ozzadar on 2025-02-17.
//
#pragma once
#include <lights/game/game_object.h>
#include <lights/scene/scene.h>
#include <lights/scene/scene_object.h>

namespace OZZ::game::scene {
    struct SpriteConstructionParams {
        std::filesystem::path VertexShaderPath{"assets/shaders/engine/sprite.vert"};
        std::filesystem::path FragShaderPath{"assets/shaders/engine/sprite.frag"};
    };

    struct SpriteUserData {
        uint64_t GameObjectID;
    };

    // TODO: @paulm mark final again after merging in configurables from AGE repo
    class Sprite : public GameObject {
    public:
        explicit Sprite(uint64_t inId,
                        GameWorld* inGameWorld,
                        const std::filesystem::path& texture,
                        const SpriteConstructionParams&& inConstructionParams = SpriteConstructionParams{});
        ~Sprite() override;
        void Tick(float DeltaTime) override;

        std::vector<OZZ::scene::SceneObject> GetSceneObjects() override;

        /**
         * Loads and uploads a texture to the sprite. Called in constructor.
         * Uses uniform name "inTexture" -- if running custom shader
         * @param inPath Path to the texture
         */
        void SetTexture(const std::filesystem::path& inPath);

        /**
         * Get the material directly. Probably unneeded unless running custom shader
         * @return the material on the main sprite
         */
        [[nodiscard]] std::shared_ptr<OZZ::Material> GetMaterial() const { return sceneObject.Mat; }

        /**
         * Creates a new body for the sprite, replaces the old one if it exists
         * @tparam Args Same arguments as OzzWorld2D::CreateBody
         * @param args  Same arguments as OzzWorld2D::CreateBody
         */
        template <typename... Args>
        void AddBody(Args&&... args) {
            const auto physicsWorldWeak = gameWorld->GetPhysicsWorld();
            if (const auto physicsWorld = physicsWorldWeak.lock()) {
                if (bodyId != InvalidBodyID) {
                    physicsWorld->DestroyBody(bodyId);
                    bodyId = InvalidBodyID;
                }

                std::any userData = SpriteUserData{.GameObjectID = id};
                bodyId = physicsWorld->CreateBody(std::forward<Args>(args)..., userData);
            }
        }

        [[nodiscard]] BodyID GetBodyID() const { return bodyId; }

        [[nodiscard]] Body* GetBody() const {
            const auto physicsWorldWeak = gameWorld->GetPhysicsWorld();
            if (const auto physicsWorld = physicsWorldWeak.lock()) {
                return physicsWorld->GetBody(bodyId);
            }
            return nullptr;
        }

    private:
        BodyID bodyId{InvalidBodyID};

        OZZ::scene::SceneObject sceneObject;
    };
} // namespace OZZ::game::scene
