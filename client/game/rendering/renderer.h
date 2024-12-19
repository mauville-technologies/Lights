//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <memory>
#include "game/scene/scene.h"
#include "buffer.h"
#include "shader.h"

namespace OZZ {
    class Renderer {
    public:
        Renderer();

        void RenderScene(Scene* scene);

    private:
        uint32_t vertexVAO;

        // TODO: Temp while testing
        std::unique_ptr<IndexVertexBuffer> mesh;
        std::unique_ptr<Shader> shader;
    };
}