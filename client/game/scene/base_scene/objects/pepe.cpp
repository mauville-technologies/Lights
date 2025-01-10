//
// Created by ozzadar on 2024-12-19.
//

#include "pepe.h"
#include "lights/rendering/shapes.h"
#include "glm/ext/matrix_transform.hpp"
#include "game/scene/constants.h"

namespace OZZ::game::scene {
    Pepe::Pepe(b2WorldId worldId) : GameObject(worldId) {
        using namespace game::constants;
        // we'll create the shape, we want it to be 5 meters wide, and 1 meter tall
        constexpr uint32_t HeightInMeters = 1;
        constexpr uint32_t WidthInMeters = 1;
        constexpr auto heightInUnits = HeightInMeters * PhysicsUnitPerMeter;
        constexpr auto widthInUnits = WidthInMeters * PhysicsUnitPerMeter;

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2BodyType::b2_dynamicBody;
        bodyDef.position = b2Vec2(0.f, 0.f);
        bodyDef.fixedRotation = true;
        bodyId = b2CreateBody(worldId, &bodyDef);

        b2Polygon box = b2MakeBox(widthInUnits/2.f, heightInUnits/2.f);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.f;
        shapeDef.friction = 0.0f;
        b2CreatePolygonShape(bodyId, &shapeDef, &box);

        // Add a little circle to his feet
        b2ShapeDef circleDef = b2DefaultShapeDef();
        circleDef.density = 0.f;
        circleDef.friction = 0.3f;
        b2Circle circleShape;
        circleShape.radius = widthInUnits/2.f;
        circleShape.center = b2Vec2(0.f, -static_cast<float>(heightInUnits/16.f));
        b2CreateCircleShape(bodyId, &circleDef, &circleShape);

        // create shader
        auto shader = std::make_shared<Shader>("assets/shaders/sprite.vert", "assets/shaders/sprite.frag");

        // create textures
//        auto image = std::make_unique<Image>("assets/textures/sprites_64/flatwoods.png");
        auto image = std::make_unique<Image>("assets/textures/pepe.png");
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

    Pepe::~Pepe() {

    }

    void Pepe::Tick(float DeltaTime) {
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

    void Pepe::Jump() {
        b2Body_ApplyForce(bodyId, b2Vec2(0.f, 500.f), b2Vec2(0.f, 0.f), true);
    }

    void Pepe::MoveLeft() {
        b2Vec2 force = b2Vec2(-500.0f, 0.0f); // Adjust the force value as needed
        b2Body_ApplyForceToCenter(bodyId, force, true);
    }

    void Pepe::MoveRight() {
        b2Vec2 force = b2Vec2(500.0f, 0.0f); // Adjust the force value as needed
        b2Body_ApplyForceToCenter(bodyId, force, true);
    }

    void Pepe::StopMoving() {
        b2Body_SetLinearVelocity(bodyId, b2Vec2(0.0f, b2Body_GetLinearVelocity(bodyId).y));
    }
}
