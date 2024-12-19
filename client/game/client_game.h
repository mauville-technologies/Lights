//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include <memory>
#include "lights/platform/window.h"
#include "game/scene/scene.h"
#include "game/rendering/renderer.h"

namespace OZZ {

    class ClientGame {
    public:
        ClientGame();

        ~ClientGame();

        void Run();

    private:
        void initWindow();
        void initGL();
        void initScene();
        void initRenderer();

        // TODO: The viewport will probably live in a render target
        void updateViewport(glm::ivec2 size);

    private:
        bool bRunning{false};
        std::shared_ptr<Window> window;

        std::unique_ptr<Scene> windowScene;
        std::unique_ptr<Renderer> renderer;
    };

} // OZZ
