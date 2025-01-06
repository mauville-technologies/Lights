//
// Created by ozzadar on 2025-01-04.
//

#pragma once
#include <tileson/tileson.h>
#include <filesystem>
#include <variant>
#include <glm/glm.hpp>


namespace std {
    template <>
    struct hash<glm::vec2> {
        size_t operator()(const glm::vec2& pos) const {
            return hash<float>()(pos.x) ^ (hash<float>()(pos.y) << 1);
        }
    };
}

namespace OZZ {

    struct TileDescription {
        enum class TileCollisionType {
            None,
            Rectangle,
            Polygon
        };
        uint32_t TileID;
        TileCollisionType CollisionType;
        glm::vec2 Position;
        std::variant<glm::vec2, std::vector<glm::vec2>> CollisionData;
        std::tuple<glm::vec2, glm::vec2> ImageRect;
    };

    struct Tileset {
        std::string Name {};
        std::filesystem::path TexturePath {};

        // should extract more properties from tileson::Tileset
        // Should have list of tiles, each with their own properties
        std::unordered_map<uint32_t, TileDescription> Tiles {};
    };

    struct TileLayer {
        std::string Name {};

        std::unordered_map<glm::vec2, uint32_t> Tiles {};
    };

    struct ObjectLayer {
        std::string Name {};
    };

    class TiledMap {
    public:
        TiledMap() = default;
        ~TiledMap() = default;

        void Load(const std::filesystem::path& path);
        void Unload();

    private:
        std::vector<Tileset> tilesets;
        std::unordered_map<std::string, TileLayer> layers;
        std::unordered_map<std::string, ObjectLayer> objectLayers;
        std::unique_ptr<tson::Map> map;
    };
} // OZZ