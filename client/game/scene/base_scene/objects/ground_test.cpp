//
// Created by ozzadar on 2025-01-01.
//

#include "ground_test.h"
#include "game/scene/constants.h"
#include "lights/rendering/shapes.h"

namespace OZZ {
    GroundTest::GroundTest(b2WorldId worldId) : GameObject(worldId) {
        using namespace game::constants;
        // we'll create the shape, we want it to be 5 meters wide, and 1 meter tall
        constexpr uint32_t HeightInMeters = 1;
        constexpr uint32_t WidthInMeters = 5;
        constexpr auto heightInUnits = HeightInMeters * PhysicsUnitPerMeter;
        constexpr auto widthInUnits = WidthInMeters * PhysicsUnitPerMeter;

        // base position
        position = { 0.f, -2.5f, 0.f };

        // Let's create physics things
        b2BodyDef groundBodyDef = b2DefaultBodyDef();
        groundBodyDef.position = b2Vec2(position.x * PhysicsUnitPerMeter, position.y * PhysicsUnitPerMeter);

        // create the body
        bodyId = b2CreateBody(worldId, &groundBodyDef);

        b2Polygon groundBox = b2MakeBox(widthInUnits/2.f, heightInUnits/2.f);

        // create the shape and add it to the body
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(bodyId, &shapeDef, &groundBox);

        // create shader
        auto shader = std::make_shared<Shader>("assets/shaders/sprite.vert", "assets/shaders/sprite.frag");

        // create textures
//        auto image = std::make_unique<Image>("assets/textures/sprites_64/flatwoods.png");
        auto image = std::make_unique<Image>("assets/textures/wall.jpg", 3);
        auto texture = std::make_shared<Texture>();
        texture->UploadData(image.get());

        // create material
        sceneObject.Mat = std::make_unique<Material>();
        sceneObject.Mat->SetShader(shader);
        sceneObject.Mat->AddTextureMapping({
                                                   .SlotName = "inTexture",
                                                   .SlotNumber = GL_TEXTURE0,
                                                   .Texture = texture
                                           });

        // create mesh
        sceneObject.Mesh = std::make_shared<IndexVertexBuffer>();
        auto vertices = std::vector<Vertex>(quadVertices.begin(), quadVertices.end());
        auto indices = std::vector<uint32_t>(quadIndices.begin(), quadIndices.end());
        sceneObject.Mesh->UploadData(vertices, indices);

        scale = {WidthInMeters, HeightInMeters, 1.f };
    }

    void GroundTest::Tick(float DeltaTime) {
        using namespace game::constants;

        // get the rotation and position from the physics body
        auto bodyTransform = b2Body_GetTransform(bodyId);
        position = { bodyTransform.p.x / PhysicsUnitPerMeter, bodyTransform.p.y / PhysicsUnitPerMeter, 0.f };

        auto& rot = bodyTransform.q;
        glm::mat4 rotationMatrix(1.0f);
        rotationMatrix[0][0] = rot.c;
        rotationMatrix[0][1] = -rot.s;
        rotationMatrix[1][0] = rot.s;
        rotationMatrix[1][1] = rot.c;

        // Create translation matrix
        auto renderScale = glm::vec3 { scale.x * PixelsPerMeter, scale.y * PixelsPerMeter, 1.f };
        glm::vec3 RenderPosition = {(position.x * PixelsPerMeter), (position.y * PixelsPerMeter), 0.f};
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), RenderPosition);
        // Create scale matrix
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), renderScale);

        // Combine to form TRS matrix
        sceneObject.Transform = translationMatrix * rotationMatrix * scaleMatrix;
    }

} // OZZ