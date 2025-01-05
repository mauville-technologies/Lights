//
// Created by ozzadar on 2025-01-04.
//

#pragma once
#include <tileson/tileson.h>
#include <filesystem>

namespace OZZ {

    struct Tileset {
        std::string Name;
        std::filesystem::path TexturePath;

        // should extract more properties from tileson::Tileset
    };

    class TiledMap {
    public:
        TiledMap() = default;
        ~TiledMap() = default;

        void Load(const std::filesystem::path& path);
        void Unload();

    private:
        std::vector<Tileset> tilesets;
        std::unique_ptr<tson::Map> map;
    };
} // OZZ