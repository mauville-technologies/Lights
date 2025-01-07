//
// Created by ozzadar on 2025-01-06.
//

#include "tilemap.h"
#include <spdlog/spdlog.h>

namespace OZZ {
    Tilemap::Tilemap(b2WorldId worldId) : GameObject(worldId) {

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

        // TODO: Build Tilesets
        for (const auto& tileset : tiledMap->GetTilesets()) {
            spdlog::info("Build tileset: {}", tileset.Name);

            // I should load the texture here

            // TODO: Also, currently there's only one tileset per map but there will need to be more sophisticated addressing
            // of tiles within tilesets in the future
        }

        // TODO: Build more than a single layer
        for (const auto& [layerName, layerInfo] : tiledMap->GetTileLayers()) {
            spdlog::info("Build layer: {}", layerName);

            //TODO: Build tiles
            for (const auto& [tilePosition, tileID] : layerInfo.Tiles) {
                spdlog::info("Building tile at: {}, {} | ID: {}", tilePosition.x, tilePosition.y, tileID);
            }
        }
    }

    void Tilemap::Tick(float DeltaTime) {

    }

} // OZZ