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
        // build tilesets
        for (auto& tileset : map->getTilesets()) {
            spdlog::info("Tileset: {}", tileset.getName());
            tilesets.push_back({
                .Name = tileset.getName(),
                .TexturePath = tileset.get<std::string>("tileset_asset_path")
            });

            // loop over tiles
            for (auto& tile : tileset.getTiles()) {
                spdlog::info("Tile: {}", tile.getId());
                // get collision information
                for (const auto& collisionObject : tile.getObjectgroup().getObjects()) {
                    spdlog::info("Collision Object: {}", collisionObject.getName());

                    auto objectType = collisionObject.getObjectType();
                    if (objectType == tson::ObjectType::Rectangle) {
                        spdlog::info("Collision Object is a rectangle");
                        auto rect = collisionObject.getSize();
                        auto position = collisionObject.getPosition();
                        spdlog::info("Collision Object AABB: x: {}, y: {}, width: {}, height: {}", position.x, position.y, rect.x, rect.y);
                        // TODO: Store this
                    } else if (objectType == tson::ObjectType::Polygon) {
                        spdlog::info("Collision Object is a polygon");
                        // print all points of polygon
                        for (const auto& point : collisionObject.getPolygons()) {
                            spdlog::info("Polygon Point: x: {}, y: {}", point.x, point.y);
                        }
                    } else {
                        spdlog::error("Collision Object is not supported {}" , (uint8_t)collisionObject.getObjectType());
                    }
                }
            }
        }

        // Print tilesets:
        for (auto& tileset : tilesets) {
            spdlog::info("Tileset: {}", tileset.Name);
            spdlog::info("Texture Path: {}", tileset.TexturePath.string());

        }

        // Let's read the map
        for (auto& layer : map->getLayers()) {
            spdlog::info("Layer: {}", layer.getName());
            if (layer.getType() == tson::LayerType::TileLayer) {
                spdlog::info("Layer is a tile layer");
                for (auto &tile: layer.getTileData()) {
//                    spdlog::info("Tile: {}", tile.second->getId());
                }
            }
            if (layer.getType() == tson::LayerType::ObjectGroup) {
                spdlog::info("Layer is an object group");
                for (auto &object: layer.getObjects()) {
//                    spdlog::info("Object: {}", object.getName());
                }
            }
        }

        spdlog::info ("Map model built successfully");
    }

    void TiledMap::Unload() {
        map.reset();
    }
} // OZZ