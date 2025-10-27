//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include <lights/game/game_world.h>
#include <lights/input/input_subsystem.h>
#include <lights/rendering/renderable.h>
#include <lights/scene/constants.h>
#include <lights/scene/scene_layer.h>
#include <lights/scene/scene_layer_manager.h>

namespace OZZ::scene {

    inline std::vector<SceneObject> operator+(std::vector<SceneObject> lhs, std::vector<SceneObject> rhs) {
        lhs.insert(lhs.end(), rhs.begin(), rhs.end());
        return lhs;
    }

    inline std::vector<SceneObject>& operator+=(std::vector<SceneObject>& lhs, const std::vector<SceneObject>& rhs) {
        lhs.insert(lhs.end(), rhs.begin(), rhs.end());
        return lhs;
    }

    class Scene {
    public:
        struct SceneParams {
            glm::vec3 ClearColor = {0.392f, 0.584f, 0.929f};
        };

        // Should be called at the end of the derived class Init function
        virtual void InitScene(std::shared_ptr<InputSubsystem> inInput);

        virtual void Tick(float DeltaTime);

        // Marked virtual to allow derived Scenes with custom entities
        virtual void WindowResized(glm::ivec2 size);

        virtual ~Scene();

        [[nodiscard]] std::vector<SceneLayer*> GetActiveLayers() const { return layerManager->GetActiveLayers(); }

        [[nodiscard]] std::vector<SceneLayer*> GetAllLayers() const { return layerManager->GetAllLayers(); }

        GameWorld* GetWorld();

        [[nodiscard]] virtual OZZ::Renderable* GetSceneGraph() const = 0;

    public:
        SceneParams Params{};

    protected:
        std::unique_ptr<SceneLayerManager> layerManager;
        std::shared_ptr<InputSubsystem> input;

    private:
        std::shared_ptr<GameWorld> world;
    };

} // namespace OZZ::scene
