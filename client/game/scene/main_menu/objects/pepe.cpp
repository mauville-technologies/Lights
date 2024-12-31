//
// Created by ozzadar on 2024-12-19.
//

#include "pepe.h"
#include "lights/rendering/shapes.h"
#include "glm/ext/matrix_transform.hpp"

namespace OZZ::game::scene {
    Pepe::Pepe() {
        // create shader
        auto shader = std::make_shared<Shader>("assets/shaders/sprite.vert", "assets/shaders/sprite.frag");

        // create textures
//        auto image = std::make_unique<Image>("assets/textures/sprites_64/flatwoods.png");
        auto image = std::make_unique<Image>("assets/textures/pepe.png");
        auto texture = std::make_shared<Texture>();
        texture->UploadData(image.get());

        // create material
        Mat = std::make_unique<Material>();
        Mat->SetShader(shader);
        Mat->AddTextureMapping({
            .SlotName = "inTexture",
            .SlotNumber = GL_TEXTURE0,
            .Texture = texture
        });

        // create mesh
        Mesh = std::make_shared<IndexVertexBuffer>();
        auto vertices = std::vector<Vertex>(quadVertices.begin(), quadVertices.end());
        auto indices = std::vector<uint32_t>(quadIndices.begin(), quadIndices.end());
        Mesh->UploadData(vertices, indices);
    }

    Pepe::~Pepe() {

    }
}