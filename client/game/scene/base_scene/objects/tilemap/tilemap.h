//
// Created by ozzadar on 2025-01-06.
//

#pragma once

#include "lights/game/game_object.h"
#include "lights/scene/scene.h"
#include "lights/game/tiled_map.h"

namespace OZZ {

    class Tilemap : public GameObject {
    public:
        class Tile {
        public:
            explicit Tile(b2WorldId worldId);
            void Tick(float DeltaTime);

            SceneObject* GetSceneObject() { return &sceneObject; }

        private:
            SceneObject sceneObject;
            b2BodyId bodyId;
        };

    public:
        explicit Tilemap(b2WorldId worldId);
        ~Tilemap();

        void Init(const std::filesystem::path& mapPath);
        void Tick(float DeltaTime) override;
        std::vector<SceneObject*> GetSceneObjects() { return {&sceneObject}; }

    private:
        SceneObject sceneObject;

        std::unique_ptr<TiledMap> tiledMap;
        // Physics things
        b2BodyId bodyId;
    };

} // OZZ
