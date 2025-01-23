//
// Created by ozzadar on 2025-01-04.
//

#include "lights/game/tiled_map.h"
#include <spdlog/spdlog.h>

namespace OZZ {
    void TiledMap::Load(const std::filesystem::path& path) {
        tson::Tileson t;
        map = t.parse(path);

        if (map->getStatus() != tson::ParseStatus::OK) {
            spdlog::error("Failed to load map: {}", map->getStatusMessage());
            Unload();
            return;
        }

        spdlog::info("Map {} loaded successfully, building map model now...", path.string());
        mapSize = {map->getSize().x, map->getSize().y};
        tileSize = {map->getTileSize().x, map->getTileSize().y};
        // build tilesets
        for (auto& tileset : map->getTilesets()) {
            spdlog::info("Tileset: {}", tileset.getName());
            tilesets.push_back({
                .Name = tileset.getName(),
                .TexturePath = tileset.get<std::string>("tileset_asset_path")
            });

            auto& NewTileset = tilesets.back();

            // loop over tiles
            for (auto& tile : tileset.getTiles()) {
                auto NewTileDescription = TileDescription {
                    .TileID = tile.getId(),
                    .CollisionType = TileDescription::TileCollisionType::None,
                };

                // spdlog::info("Tile: {}", tile.getId());
                // get collision information
                for (const auto& collisionObject : tile.getObjectgroup().getObjects()) {
                    auto objectType = collisionObject.getObjectType();
                    if (objectType == tson::ObjectType::Rectangle) {
                        // spdlog::info("Collision Object is a rectangle");
                        auto rect = collisionObject.getSize();
                        auto position = collisionObject.getPosition();
                        // spdlog::info("Collision Object AABB: x: {}, y: {}, width: {}, height: {}", position.x, position.y, rect.x, rect.y);

                        NewTileDescription.CollisionType = TileDescription::TileCollisionType::Rectangle;
                        NewTileDescription.Position = {position.x, position.y};
                        NewTileDescription.CollisionData = glm::vec2(rect.x, rect.y);
                    } else if (objectType == tson::ObjectType::Polygon) {
                        // spdlog::info("Collision Object is a polygon");
                        // print all points of polygon
                        std::vector<glm::vec2> points;
                        for (const auto& point : collisionObject.getPolygons()) {
                            // spdlog::info("Polygon Point: x: {}, y: {}", point.x, point.y);
                            points.emplace_back(point.x, point.y);
                        }

                        NewTileDescription.CollisionType = TileDescription::TileCollisionType::Polygon;
                        NewTileDescription.Position = {collisionObject.getPosition().x, collisionObject.getPosition().y};
                        NewTileDescription.CollisionData = points;
                    } else {
                        spdlog::error("Collision Object is not supported {}" , (uint8_t)collisionObject.getObjectType());
                    }

                    auto NewImageRect = tile.getDrawingRect();
                    NewTileDescription.ImageRect = {glm::vec2(NewImageRect.x, NewImageRect.y), glm::vec2(NewImageRect.width, NewImageRect.height)};
                    // spdlog::info("Image Rect: x: {}, y: {}, width: {}, height: {}", NewImageRect.x, NewImageRect.y, NewImageRect.width, NewImageRect.height);
                    NewTileset.Tiles[tile.getId()] = NewTileDescription;
                }
            }

        }

        // Let's read the map
        for (auto& layer : map->getLayers()) {
            spdlog::info("Layer: {}", layer.getName());
            if (layer.getType() == tson::LayerType::TileLayer) {
                auto NewTileLayer = TileLayer {
                    .Name = layer.getName()
                };

                // spdlog::info("Layer is a tile layer");
                for (auto &tile: layer.getTileData()) {
                    auto& [x, y] = tile.first;
                    auto tileID = tile.second->getId();

                    NewTileLayer.Tiles[{x, y}] = tileID;
                }
                layers[layer.getName()] = NewTileLayer;
            }
            if (layer.getType() == tson::LayerType::ObjectGroup) {
                // spdlog::info("Layer is an object group");
                for (auto &object: layer.getObjects()) {
                    // TODO: implement object layer
                    // NOTE: objects can be of various types. I'll support the ones
                    // I use for now and then expand as needed
                }
            }
        }

        spdlog::info ("Map model built successfully");
    }

    void TiledMap::Unload() {
        spdlog::info("Unloading Tiledmap");
        map.reset();
    }
} // OZZ