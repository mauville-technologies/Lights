//
// Created by ozzadar on 2025-02-17.
//
#pragma once
#include <lights/game/game_object.h>
#include <lights/scene/scene.h>
#include <unordered_map>
#include <string>

namespace OZZ::game::scene {
    class Sprite final : public GameObject {
    public:
        explicit Sprite(std::shared_ptr<OzzWorld2D> inWorld, const std::filesystem::path& texture);
        void Tick(float DeltaTime) override;

        std::vector<SceneObject> GetSceneObjects() override;

        void SetTexture(const std::filesystem::path& inPath);
    private:
        bool bDrawDebug = true;

        SceneObject sceneObject;

        static std::unordered_map<std::string, SceneObject> debugShapes;
        static std::shared_ptr<Shader> debugShader;
    };
}
