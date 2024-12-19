//
// Created by ozzadar on 2024-12-18.
//

#include "renderer.h"
#include "glad/glad.h"


namespace OZZ {
    void Renderer::RenderScene(Scene *scene) {
        // TODO: Bind the render target

        // cornflower blue
        glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO: Render the scene

        // TODO: Unbind the render target
    }
}