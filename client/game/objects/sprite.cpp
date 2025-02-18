//
// Created by ozzadar on 2025-02-17.
//

#include "sprite.h"

#include <lights/rendering/shapes.h>

namespace OZZ::game::scene {
    Sprite::Sprite(std::shared_ptr<OzzWorld2D> inWorld, const std::filesystem::path& texture) : GameObject(std::move(inWorld)) {
        const auto shader = std::make_shared<Shader>("assets/shaders/sprite.vert", "assets/shaders/sprite.frag");
        sceneObject.Mat = std::make_unique<Material>();
        sceneObject.Mat->SetShader(shader);
        sceneObject.Mesh = std::make_shared<IndexVertexBuffer>();
        auto vertices = std::vector<Vertex>(quadVertices.begin(), quadVertices.end());
        auto indices = std::vector<uint32_t>(quadIndices.begin(), quadIndices.end());
        sceneObject.Mesh->UploadData(vertices, indices);

        SetTexture(texture);
        Scale = {1.f, 1.f, 1.f};
    }

    void Sprite::Tick(float DeltaTime) {}

    std::vector<SceneObject> Sprite::GetSceneObjects() {
        /* TODO: This is going to be a terrible implementation
         * Later I will want to cache the transform but for now we'll rebuild it every frame
         */

        glm::mat4 transform(1.f);
        glm::vec3 renderPosition = {
            Position.x * constants::PixelsPerMeter,
            Position.y * constants::PixelsPerMeter,
            0.f
        };

        transform = glm::translate(transform, renderPosition);

        glm::vec3 renderScale = {
            Scale.x * constants::PixelsPerMeter,
            Scale.y * constants::PixelsPerMeter,
            1.f
        };

        transform = glm::scale(transform, renderScale);
        sceneObject.Transform = transform;

        return { sceneObject };
    }

    void Sprite::SetTexture(const std::filesystem::path &inPath) {
        const auto image = std::make_unique<Image>(inPath);
        const auto texture = std::make_shared<Texture>();
        texture->UploadData(image.get());

        sceneObject.Mat->AddTextureMapping({
            .SlotName = "inTexture",
            .SlotNumber = GL_TEXTURE0,
            .TextureResource = texture
        });
    }
}
