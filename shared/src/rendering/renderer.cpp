//
// Created by ozzadar on 2024-12-18.
//

#include "lights/rendering/renderer.h"
#include "glad/glad.h"

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
        for (auto& layer : scene->GetLayers()) {
            for (auto &object: layer->GetSceneObjects()) {
                auto &[transform, objMesh, objMat] = object;
                objMat->Bind();
                objMat->GetShader()->SetMat4("view", layer->LayerCamera.ViewMatrix);
                objMat->GetShader()->SetMat4("projection", layer->LayerCamera.ProjectionMatrix);
                objMat->GetShader()->SetMat4("model", transform);
                objMesh->Bind();
                auto drawMode = ToGLEnum(objMat->GetDrawMode());
                glDrawElements(drawMode, objMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
            }
        }

        // TODO: Unbind the render target
    }
}