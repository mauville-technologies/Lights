//
// Created by ozzadar on 2025-02-17.
//
#pragma once
#include <lights/game/game_object.h>
#include <lights/scene/scene.h>
#include <lights/scene/scene_object.h>
#include <string>
#include <unordered_map>

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
                        std::shared_ptr<OzzWorld2D> inPhysicsWorld,
                        const std::filesystem::path& texture,
                        const SpriteConstructionParams&& inConstructionParams = SpriteConstructionParams{});
        ~Sprite() override;
        void Tick(float DeltaTime) override;

        std::vector<OZZ::scene::SceneObject> GetSceneObjects() override;

        /**
         * Loads and uploads a texture to the sprite. Called in constructor.
         * Uses uniform name "inTexture" -- if running custom shader
         * @param inPath Path to the texture
         * @param imageChannels Number of channels on the source image
         */
        void SetTexture(const std::filesystem::path& inPath);

        /**
         * Get the material directly. Probably unneeded unless running custom shader
         * @return the material on the main sprite
         */
        std::shared_ptr<OZZ::Material> GetMaterial() { return sceneObject.Mat; }

        /**
         * Creates a new body for the sprite, replaces the old one if it exists
         * @tparam Args Same arguments as OzzWorld2D::CreateBody
         * @param args  Same arguments as OzzWorld2D::CreateBody
         */
        template <typename... Args>
        void AddBody(Args&&... args) {
            if (bodyId != InvalidBodyID) {
                physicsWorld->DestroyBody(bodyId);
                bodyId = InvalidBodyID;
            }

            std::any userData = SpriteUserData{.GameObjectID = id};
            bodyId = physicsWorld->CreateBody(std::forward<Args>(args)..., userData);
        }

        [[nodiscard]] BodyID GetBodyID() const { return bodyId; }

        [[nodiscard]] Body* GetBody() const { return physicsWorld->GetBody(bodyId); }

    private:
        BodyID bodyId{InvalidBodyID};

#ifdef OZZ_DEBUG
        bool bDrawDebug = true;
#else
        bool bDrawDebug = false;
#endif

        float debugDrawSize = 2;

        OZZ::scene::SceneObject sceneObject;

        static std::unordered_map<std::string, OZZ::scene::SceneObject> debugShapes;
        static std::shared_ptr<Shader> debugShader;
    };
} // namespace OZZ::game::scene
