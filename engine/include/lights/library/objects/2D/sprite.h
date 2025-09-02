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
        uint8_t TextureImageChannels{4};
        std::filesystem::path VertexShaderPath{"assets/shaders/engine/sprite.vert"};
        std::filesystem::path FragShaderPath{"assets/shaders/engine/sprite.frag"};
    };

    class Sprite final : public GameObject {
    public:
        explicit Sprite(GameWorld* inGameWorld,
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
        void SetTexture(const std::filesystem::path& inPath, uint8_t imageChannels);

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
            if (MainBody != InvalidBodyID) {
                physicsWorld->DestroyBody(MainBody);
                MainBody = InvalidBodyID;
            }
            MainBody = physicsWorld->CreateBody(std::forward<Args>(args)...);
        }

        [[nodiscard]] Body* GetBody() const { return physicsWorld->GetBody(MainBody); }

    private:
        BodyID MainBody{InvalidBodyID};

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
