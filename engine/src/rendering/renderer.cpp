//
// Created by ozzadar on 2024-12-18.
//

#include "lights/rendering/renderer.h"
#include "glad/glad.h"

namespace OZZ {
    Renderer::Renderer() {}

    void Renderer::Init() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glFrontFace(GL_CCW);
        glEnable(GL_CULL_FACE);
    }

    void Renderer::RenderScene(scene::Scene* scene) {
        const auto [ClearColor] = scene->Params;
        glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO: Render the scene
        // Render all scene objects
        for (auto& layer : scene->GetLayers()) {
            for (auto& object : layer->GetSceneObjects()) {
                auto& [transform, objMesh, objMat] = object;
                objMat->Bind();
                objMat->GetShader()->SetMat4("view", layer->LayerCamera.ViewMatrix);
                objMat->GetShader()->SetMat4("projection", layer->LayerCamera.ProjectionMatrix);
                objMat->GetShader()->SetMat4("model", transform);
                objMesh->Bind();
                const auto drawMode = ToGLEnum(objMat->GetSettings().Mode);
                glDrawElements(drawMode, objMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
            }
        }

        // TODO: Unbind the render target
    }
}
