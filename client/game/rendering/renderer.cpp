//
// Created by ozzadar on 2024-12-18.
//

#include "renderer.h"
#include "glad/glad.h"
#include "spdlog/spdlog.h"
#include "shapes.h"

namespace OZZ {
    Renderer::Renderer() {

    }

    void Renderer::RenderScene(Scene *scene) {
        // TODO: Bind the render target
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO: Render the scene
        // Render all scene objects
        for (auto& object: scene->GetObjects()) {
            auto& [transform, objMesh, objMat] = *object;
            objMat->Bind();
            objMat->GetShader()->SetMat4("view", scene->GetCamera()->ViewMatrix);
            objMat->GetShader()->SetMat4("projection", scene->GetCamera()->ProjectionMatrix);
            objMat->GetShader()->SetMat4("model", transform);
            objMesh->Bind();
            glDrawElements(GL_TRIANGLES, objMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
        }


        // TODO: Unbind the render target
    }
}