//
// Created by ozzadar on 2025-02-17.
//

#include <lights/library/objects/2D/sprite.h>
#include <lights/rendering/shapes.h>
#include <lights/scene/scene_object.h>

namespace OZZ::game::scene {
    std::shared_ptr<Shader> Sprite::debugShader = nullptr;
    std::unordered_map<std::string, OZZ::scene::SceneObject> Sprite::debugShapes{};

    Sprite::Sprite(uint64_t inId,
                   GameWorld* inGameWorld,
                   std::shared_ptr<OzzWorld2D> inPhysicsWorld,
                   const std::filesystem::path& texture,
                   const SpriteConstructionParams&& inConstructionParams)
        : GameObject(inId, inGameWorld, std::move(inPhysicsWorld)) {
        const auto shader =
            std::make_shared<Shader>(inConstructionParams.VertexShaderPath, inConstructionParams.FragShaderPath);
        sceneObject.Mat = std::make_unique<Material>();
        sceneObject.Mat->SetShader(shader);
        sceneObject.Mesh = std::make_shared<IndexVertexBuffer>();
        auto vertices = std::vector<Vertex>(quadVertices.begin(), quadVertices.end());
        auto indices = std::vector<uint32_t>(quadIndices.begin(), quadIndices.end());
        sceneObject.Mesh->UploadData(vertices, indices);

        SetTexture(texture, inConstructionParams.TextureImageChannels);
        SetScale({1.f, 1.f, 1.f});
    }

    Sprite::~Sprite() {
        if (bodyId != InvalidBodyID) {
            physicsWorld->DestroyBody(bodyId);
            bodyId = InvalidBodyID;
        }
    }

    void Sprite::Tick(float DeltaTime) {
        // Update gameobject position to match main body position
        if (const auto body = physicsWorld->GetBody(bodyId)) {
            // TODO: The rendering position and scale might need to be separate from the physics position and scale
            // this will work for now but I'll have to think about it more later
            SetPosition(body->GetPosition());
            SetScale({body->GetScale(), 1.f});
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

        if (const auto mainBody = physicsWorld->GetBody(bodyId)) {
            auto ShapePosition = mainBody->GetPosition();
            renderPosition.x = ShapePosition.x;
            renderPosition.y = ShapePosition.y;
        }

        transform = glm::translate(transform, renderPosition);

        glm::vec3 renderScale = {scale.x, scale.y, 1.f};

        transform = glm::scale(transform, renderScale);
        sceneObject.Transform = transform;

        objects.emplace_back(sceneObject);

        // let's add the needed debug shapes here
        if (bDrawDebug) {
            // The static lazy initialization of this is a bit ugly. Likely there should be a resource manager available
            // somehow that can initialize and cache these resources. For now, this will do -- but will likely break if
            // changing scenes. These are very small things to sit in memory so it might not be a problem that these
            // will stick around for the lifetime of the application.
            // TODO: If the glcontext changes (I don't know why it would), this will probably break.

            if (!debugShader) {
                debugShader =
                    std::make_shared<Shader>("assets/shaders/engine/debug.vert", "assets/shaders/engine/debug.frag");
                debugShader->Bind();
                // red
                debugShader->SetVec3("lineColor", {1.f, 0.f, 0.f});
            }

            // TODO: There's a lot of repeated code in these if statements. A refactor here would be good.
            if (!debugShapes.contains("circle")) {
                auto [vertices, indices] = GenerateCircle(0.5f, 16, true);
                const auto circleMesh = std::make_shared<IndexVertexBuffer>();
                circleMesh->UploadData(vertices, indices);

                OZZ::scene::SceneObject circleObject;
                circleObject.Mat = std::make_unique<Material>();
                circleObject.Mat->SetShader(debugShader);
                circleObject.Mat->GetSettings().Mode = DrawMode::Lines;
                circleObject.Mat->GetSettings().LineWidth = debugDrawSize;
                circleObject.Mat->GetSettings().PointSize = debugDrawSize;
                circleObject.Mesh = circleMesh;

                debugShapes["circle"] = circleObject;
            }

            if (!debugShapes.contains("quad")) {
                auto vertices = std::vector<Vertex>(quadVertices.begin(), quadVertices.end());
                auto indices = std::vector<uint32_t>(quadOutlineIndices.begin(), quadOutlineIndices.end());

                const auto quadMesh = std::make_shared<IndexVertexBuffer>();
                quadMesh->UploadData(vertices, indices);

                OZZ::scene::SceneObject quadObject;
                quadObject.Mat = std::make_unique<Material>();
                quadObject.Mat->SetShader(debugShader);
                quadObject.Mat->GetSettings().LineWidth = debugDrawSize;
                quadObject.Mat->GetSettings().PointSize = debugDrawSize;
                quadObject.Mat->GetSettings().Mode = DrawMode::Lines;
                quadObject.Mesh = quadMesh;

                debugShapes["quad"] = quadObject;
            }

            if (!debugShapes.contains("point")) {
                auto vertices = std::vector<Vertex>{Vertex{
                    .position = {0.f, 0.f, 0.f},
                    .color = {1.f, 0.f, 0.f, 0.f},
                    .normal = {0.f, 0.f, 1.f},
                    .uv = {0.f, 0.f},
                }};

                auto indices = std::vector<uint32_t>{0};
                const auto pointMesh = std::make_shared<IndexVertexBuffer>();
                pointMesh->UploadData(vertices, indices);

                OZZ::scene::SceneObject pointObject;
                pointObject.Mat = std::make_unique<Material>();
                pointObject.Mat->SetShader(debugShader);
                pointObject.Mat->GetSettings().Mode = DrawMode::Points;
                pointObject.Mat->GetSettings().PointSize = debugDrawSize;
                pointObject.Mesh = pointMesh;

                debugShapes["point"] = pointObject;
            }

            // For each body in the Bodies array, let's draw its debug shape
            if (auto* body = physicsWorld->GetBody(bodyId)) {
                if (body->bCollidedThisFrame) {
                    debugShader->SetVec3("lineColor", {0.f, 1.f, 0.f});
                } else {
                    debugShader->SetVec3("lineColor", {1.f, 0.f, 0.f});
                }
                const auto shapeKind = static_cast<OzzShapeKind>(body->Data.index());
                switch (shapeKind) {
                    case OzzShapeKind::Circle: {
                        // build transform from shape
                        auto& circleShape = std::get<OzzCircle>(body->Data);
                        glm::mat4 circleTransform{1.f};
                        glm::vec3 circleRenderPosition{circleShape.Position.x, circleShape.Position.y, 0.f};
                        glm::vec3 circleRenderScale{circleShape.Radius * 2, circleShape.Radius * 2, 0.f};

                        circleTransform = glm::translate(circleTransform, circleRenderPosition);
                        circleTransform = glm::scale(circleTransform, circleRenderScale);

                        // create a copy
                        auto& circleObject = objects.emplace_back(debugShapes["circle"]);
                        circleObject.Transform = circleTransform;
                        break;
                    }
                    case OzzShapeKind::Rectangle: {
                        // build transform from shape
                        auto& rectangleShape = std::get<OzzRectangle>(body->Data);
                        glm::mat4 rectangleTransform{1.f};
                        glm::vec3 rectangleRenderPosition{rectangleShape.Position.x, rectangleShape.Position.y, 0.f};
                        glm::vec3 rectangleRenderScale{rectangleShape.Size.x, rectangleShape.Size.y, 1.f};

                        rectangleTransform = glm::translate(rectangleTransform, rectangleRenderPosition);
                        rectangleTransform = glm::scale(rectangleTransform, rectangleRenderScale);

                        // create a copy
                        auto& rectangleObject = objects.emplace_back(debugShapes["quad"]);
                        rectangleObject.Transform = rectangleTransform;
                        break;
                    }
                    case OzzShapeKind::Point:
                        break;
                    default:
                        break;
                }
            }
        }

        return objects;
    }

    void Sprite::SetTexture(const std::filesystem::path& inPath, uint8_t imageChannels) {
        const auto image = std::make_unique<Image>(inPath, imageChannels);
        const auto texture = std::make_shared<Texture>();
        texture->UploadData(image.get());

        sceneObject.Mat->AddTextureMapping(
            {.SlotName = "inTexture", .SlotNumber = GL_TEXTURE0, .TextureResource = texture});
    }
} // namespace OZZ::game::scene
