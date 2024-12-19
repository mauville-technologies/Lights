//
// Created by ozzadar on 2024-12-18.
//

#include "renderer.h"
#include "glad/glad.h"


namespace OZZ {
    Renderer::Renderer() {
        // Currently only one vertex format, so we'll use a single VAO for the entire application
//        glGenVertexArrays(1, &vertexVAO);
//        glBindVertexArray(vertexVAO);
//        Vertex::BindAttribPointers();

        shader = std::make_unique<Shader>("assets/shaders/basic.vert", "assets/shaders/basic.frag");
        // make a square
        std::vector<Vertex> vertices = {
                {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.5f, 0.5f}}
        };

        std::vector<uint32_t> indices = {
                0, 1, 3,
                1, 2, 3
        };

        mesh = std::make_unique<IndexVertexBuffer>();
        mesh->UploadData(vertices, indices);
    }

    void Renderer::RenderScene(Scene *scene) {
        // TODO: Bind the render target

        // cornflower blue
//        glBindVertexArray(vertexVAO);
        glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO: Render the scene
        shader->Bind();
        mesh->Bind();

        glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
        // TODO: Unbind the render target
    }
}