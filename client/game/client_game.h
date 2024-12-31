//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include <memory>
#include "lights/platform/window.h"
#include "lights/rendering/renderer.h"
#include "lights/input/input_subsystem.h"
#include "lights/scene/scene.h"
#include "lights/ui/user_interface.h"
#include "network/client.h"
#include "application_state.h"

namespace OZZ::game {

    class ClientGame {
    public:
        ClientGame();

        ~ClientGame();

        void Run();

    private:
        void initWindow();
        void initGL();
        void initInput();
        void initScene();
        void initRenderer();
        void initUI();
        void initNetwork();

        // TODO: The viewport will probably live in a render target
        void updateViewport(glm::ivec2 size);

        void drawScene(Scene* scene);
    private:
        bool bRunning{false};

        ApplicationState appState;

        std::shared_ptr<Window> window;
        std::shared_ptr<InputSubsystem> input;
        std::shared_ptr<UserInterface> ui;

        std::unique_ptr<Scene> windowScene;
        std::unique_ptr<Renderer> renderer;

        std::thread networkThread;
        std::unique_ptr<network::client::Client> client;
    };

} // OZZ
