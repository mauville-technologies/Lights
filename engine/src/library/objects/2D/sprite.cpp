//
// Created by ozzadar on 2025-02-17.
//

#include <lights/library/objects/2D/sprite.h>
#include <lights/rendering/shapes.h>
#include <lights/scene/scene_object.h>

namespace OZZ::game::scene {
    Sprite::Sprite(uint64_t inId,
                   GameWorld* inGameWorld,
                   const std::filesystem::path& texture,
                   const SpriteConstructionParams&& inConstructionParams)
        : GameObject(inId, inGameWorld) {
        const auto shader = std::make_shared<Shader>(inConstructionParams.VertexShaderPath.string(),
                                                     inConstructionParams.FragShaderPath.string());
        sceneObject.Mat = std::make_unique<Material>();
        sceneObject.Mat->SetShader(shader);
        sceneObject.Mesh = std::make_shared<IndexVertexBuffer>();
        auto vertices = OZZ::quadVertices | std::views::transform([](Vertex v) {
                            v.position *= 0.5f;
                            return v;
                        }) |
                        std::ranges::to<std::vector<Vertex>>();
        auto indices = std::vector<uint32_t>(quadIndices.begin(), quadIndices.end());
        sceneObject.Mesh->UploadData(vertices, indices);

        SetTexture(texture);
        SetScale({1.f, 1.f, 1.f});
    }

    Sprite::~Sprite() {
        if (bodyId != InvalidBodyID) {
            if (const auto physicsWorld = gameWorld->GetPhysicsWorld().lock()) {
                physicsWorld->DestroyBody(bodyId);
            }
            bodyId = InvalidBodyID;
        }
    }

    void Sprite::Tick(float DeltaTime) {
        // Update gameobject position to match main body position
        if (const auto physicsWorld = gameWorld->GetPhysicsWorld().lock()) {
            if (const auto body = physicsWorld->GetBody(bodyId)) {
                // TODO: The rendering position and scale might need to be separate from the physics position and scale
                // this will work for now but I'll have to think about it more later
                SetPosition(body->GetPosition());
                SetScale({body->GetScale(), 1.f});
            }
        }
    }

    std::vector<OZZ::scene::SceneObject> Sprite::GetSceneObjects() {
        std::vector<OZZ::scene::SceneObject> objects;

        /* TODO: This is going to be a terrible implementation
         * Later I will want to cache the transform but for now we'll rebuild it every frame
         */
        glm::mat4 transform(1.f);
        const auto& position = GetPosition();
        const auto& scale = GetScale();
        glm::vec3 renderPosition = {position.x, position.y, 0.f};

        if (const auto physicsWorld = gameWorld->GetPhysicsWorld().lock()) {
            if (const auto mainBody = physicsWorld->GetBody(bodyId)) {
                const auto ShapePosition = mainBody->GetPosition();
                renderPosition.x = ShapePosition.x;
                renderPosition.y = ShapePosition.y;
            }
        }

        transform = glm::translate(transform, renderPosition);

        glm::vec3 renderScale = {scale.x, scale.y, 1.f};

        transform = glm::scale(transform, renderScale);
        sceneObject.Transform = transform;

        objects.emplace_back(sceneObject);

        return objects;
    }

    void Sprite::SetTexture(const std::filesystem::path& inPath) {
        const auto image = std::make_unique<Image>(inPath);
        const auto texture = std::make_shared<Texture>();
        texture->UploadData(image.get());

        sceneObject.Mat->AddTextureMapping(
            {.SlotName = "inTexture", .SlotNumber = GL_TEXTURE0, .TextureResource = texture});
    }
} // namespace OZZ::game::scene
