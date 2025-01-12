//
// Created by ozzadar on 2025-01-06.
//

#include "tilemap.h"
#include <spdlog/spdlog.h>
#include "lights/rendering/shapes.h"

#include <utility>
#include <game/scene/units.h>

namespace OZZ {
    Tilemap::Tile::Tile(glm::vec2 inTilemapPosition, TileDescription inDescription,
                        const TileResources &resources) : description(std::move(inDescription)),
                                                          tilemapPosition(inTilemapPosition) {
        // Set up the material
        // Create the material for the tile.
        sceneObject.Mat = std::make_shared<Material>();
        sceneObject.Mat->SetShader(resources.Shader);
        sceneObject.Mat->AddTextureMapping(TextureMapping{
            .SlotName = "inTexture",
            .SlotNumber = GL_TEXTURE0,
            .Texture = resources.TilemapTexture,
        });

        // Set up the mesh
        // create the mesh for the tile
        auto vertices = std::vector<Vertex>(quadVertices.begin(), quadVertices.end());
        auto indices = std::vector<uint32_t>(quadIndices.begin(), quadIndices.end());

        auto textureMapWidth = resources.TilemapTexture->GetWidth();
        auto textureMapHeight = resources.TilemapTexture->GetHeight();

        const auto &[tmPosition, tileSize] = description.ImageRect;

        auto minU = tmPosition.x / static_cast<float>(textureMapWidth);
        auto minV = (static_cast<float>(textureMapHeight) - (tmPosition.y + tileSize.y)) / static_cast<float>(
                        textureMapHeight);
        auto maxU = (tmPosition.x + tileSize.x) / static_cast<float>(textureMapWidth);

        auto maxV = (static_cast<float>(textureMapHeight) - tmPosition.y) / static_cast<float>(textureMapHeight);

        vertices[0].uv = {maxU, maxV};
        vertices[1].uv = {maxU, minV};
        vertices[2].uv = {minU, minV};
        vertices[3].uv = {minU, maxV};

        sceneObject.Mesh = std::make_shared<IndexVertexBuffer>();
        sceneObject.Mesh->UploadData(vertices, indices);

        auto PixelsPosition = units::MetersToPixels(tilemapPosition);
        // remove half the tilesize to center the tile
        PixelsPosition.x -= tileSize.x / 2.f;
        PixelsPosition.y -= tileSize.y / 2.f;
        // translate and scale the tile to the correct position
        sceneObject.Transform = glm::translate(glm::mat4{1.f},
                                               glm::vec3(PixelsPosition.x,
                                                         PixelsPosition.y, 0.f));
        sceneObject.Transform = glm::scale(sceneObject.Transform, glm::vec3(tileSize.x, tileSize.y, 1.f));

    }

    Tilemap::Tilemap() : GameObject() {
        transform = glm::scale(glm::mat4{1.f}, glm::vec3(1.f, 1.f, 1.f));
    }

    Tilemap::~Tilemap() {
        if (tiledMap) {
            tiledMap->Unload();
            tiledMap.reset();
        }
    }

    void Tilemap::Init(const std::filesystem::path &mapPath) {
        tiledMap = std::make_unique<TiledMap>();
        tiledMap->Load(mapPath);

        tileShader = std::make_shared<Shader>("assets/shaders/sprite.vert", "assets/shaders/sprite.frag");

        buildTiles();

        middleOfMapTranslation = glm::vec2{
            -(tiledMap->GetMapSize().x / 2.f) * tiledMap->GetTileSize().x,
            (tiledMap->GetMapSize().y / 2.f) * tiledMap->GetTileSize().y};


        // build the map collision and add it to the world
        buildCollision();
    }

    void Tilemap::Tick(float DeltaTime) {
    }

    std::vector<SceneObject> Tilemap::GetSceneObjects() {
        std::vector<SceneObject> objects;
        for (auto &tile: tiles) {
            // make a copy of the scene object
            auto sceneObject = *tile.GetSceneObject();
            // apply the tilemap transform to it to ensure proper positioning
            auto tilemapTransform = glm::translate(glm::mat4{1.f}, glm::vec3(middleOfMapTranslation, 0.f));
            sceneObject.Transform = tilemapTransform * sceneObject.Transform;
            objects.emplace_back(sceneObject);
        }

        return objects;
    }

    void Tilemap::buildTiles() {
        //TODO: this is currently a bunch of different index buffers. It should be a single index buffer
        for (const auto &tileset: tiledMap->GetTilesets()) {
            spdlog::info("Build tileset: {}", tileset.Name);

            // load the image
            auto tilesetImage = Image{tileset.TexturePath};

            // create the texture
            auto tilesetTexture = std::make_shared<Texture>();
            tilesetTexture->UploadData(&tilesetImage);

            if (tilesetTextures.contains(tileset.Name)) {
                spdlog::warn("Tileset {} already exists in the texture map -- replacing", tileset.Name);
                // we remove it to trigger its destruction explicitly.
                tilesetTextures.erase(tileset.Name);
            }

            tilesetTextures[tileset.Name] = tilesetTexture;
        }

        // TODO: Build more than a single layer
        for (const auto &[layerName, layerInfo]: tiledMap->GetTileLayers()) {
            spdlog::info("Build layer: {}", layerName);

            //TODO: Build tiles
            for (const auto &[tilePosition, tileID]: layerInfo.Tiles) {
                // spdlog::info("Building tile at: {}, {} | ID: {}", tilePosition.x, tilePosition.y, tileID);
                const auto &Tileset = tiledMap->GetTilesets().at(0);
                const auto &TileInformation = Tileset.Tiles.at(tileID);
                const auto &Texture = tilesetTextures.at(Tileset.Name);

                // Create a tile with that texture, position, and collision information
                tiles.emplace_back(tilePosition, TileInformation, TileResources{
                                       .TilemapTexture = Texture,
                                       .Shader = tileShader,
                                   });
            }
        }
    }

    void Tilemap::buildCollision() {
        /*
         *  TODO: Investigate this further.
         *
         *  Okay, so for now we're going to just take all the tile information, then build a single body for the entire
         *  thing. My understanding is that there may be collision problems with this approach, but we'll see. Specifically,
         *  having a bunch of tiles side-by-side to do collision apparently need to be marked as "chained together" or something.
         *
         *  Let's just get *something* working for now and we'll refine later
         */

        // First let's create the body
        auto tilemapPhysicsMiddle = units::PixelsToPhysics(middleOfMapTranslation);
        tilemapPhysicsMiddle.y = -tilemapPhysicsMiddle.y;

        /*
         *  TODO: All tiles are being treated as static here. When I start spawning objects or tiles with possible dynamic properties i'll need to something different
         *  I'm making a bunch of boxes here, which is causing the dude to get stuck on corners. I need to build out the "landmasses" in the map so that the player can walk on it properly
         */
        for (const auto& Tile : tiles) {
            switch (Tile.GetDescription().CollisionType) {
                case TileDescription::TileCollisionType::Rectangle:
                    // make a box of the correct size and position it correctly
                        auto tileSize = std::get<glm::vec2>(Tile.GetDescription().CollisionData);
                        auto physicsSize = units::PixelsToPhysics(tileSize);
                        physicsSize.x = std::abs(physicsSize.x);
                        physicsSize.y = std::abs(physicsSize.y);

                        auto tilePosition = units::PixelsToPhysics((Tile.GetTilemapPosition() - glm::vec2{0.5, -0.5})* tileSize) + tilemapPhysicsMiddle;

                        // TODO: PHYSICS
                    break;
                case TileDescription::TileCollisionType::Polygon:
                    break;
                case TileDescription::TileCollisionType::None:
                    spdlog::error("Tile type not supported for collision: {}", (uint8_t)Tile.GetDescription().CollisionType);
                    break;
            }
        }
    }
} // OZZ
