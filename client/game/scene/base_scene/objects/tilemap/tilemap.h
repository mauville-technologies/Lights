//
// Created by ozzadar on 2025-01-06.
//

#pragma once

#include "lights/game/game_object.h"
#include "lights/scene/scene.h"
#include "lights/game/tiled_map.h"

#include <memory>

namespace OZZ {

    class Tilemap : public GameObject {
    public:
        struct TileResources {
            std::shared_ptr<Texture> TilemapTexture;
            std::shared_ptr<Shader> TileShader;
        };

        class Tile {
        public:
            explicit Tile(glm::vec2 inTilemapPosition, TileDescription  description, const TileResources& resources);
            ~Tile() = default;
            void Tick(float DeltaTime);

            SceneObject* GetSceneObject() { return &sceneObject; }

            [[nodiscard]] const TileDescription& GetDescription() const { return description; }
            [[nodiscard]] const glm::vec2& GetTilemapPosition() const { return tilemapPosition; }
        private:
            TileDescription description;
            glm::vec2 tilemapPosition;
            std::shared_ptr<Material> material;

            SceneObject sceneObject;
        };

    public:
        explicit Tilemap(std::shared_ptr<CollisionSystem> InCollision);
        ~Tilemap() override;

        void Init(const std::filesystem::path& mapPath);
        void Tick(float DeltaTime) override;
        std::vector<SceneObject> GetSceneObjects();

    private:
        void buildTiles();
        void buildCollision();

    private:
        glm::vec2 middleOfMapTranslation {};
        glm::vec2 sceneTranslation {0.f};
        glm::mat4 transform {1.f};
        std::shared_ptr<Shader> tileShader;

        std::unique_ptr<TiledMap> tiledMap;
        std::unordered_map<std::string, std::shared_ptr<Texture>> tilesetTextures;


        std::vector<Tile> tiles;
    };

} // OZZ
