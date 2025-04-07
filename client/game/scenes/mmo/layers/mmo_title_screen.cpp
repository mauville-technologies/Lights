//
// Created by ozzadar on 2025-04-06.
//

#include "mmo_title_screen.h"

#include <lights/rendering/shapes.h>

void MMOTitleScreen::Init() {
	SceneLayer::Init();
	LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), // Camera position
	                                     glm::vec3(0.f, 0.f, 0.f), // Target to look at
	                                     glm::vec3(0.f, 1.f, 0.f)); // Up vector

	fontLoader = std::make_unique<OZZ::FontLoader>();
	alphabetCharacterData =
		fontLoader->GetString(fontPath,
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+[]{}|;':\",.<>?`~ ",
			32);

	// put a log line in here so i can break on it
	spdlog::info("MMOTitleScreen::Init() - Font loaded");

	const auto shader = std::make_shared<OZZ::Shader>("assets/shaders/font.vert", "assets/shaders/font.frag");
	// create the title screen quads
	for (const auto [character, characterData]: alphabetCharacterData) {
		// create title screen material
		const auto texture = std::make_shared<OZZ::Texture>();
		characterData->Texture->FlipPixels(true, false);
		texture->UploadData(characterData->Texture.get());
		const auto mat = std::make_shared<OZZ::Material>();
		mat->SetShader(shader);
		mat->AddTextureMapping({
			.SlotName = "inTexture",
			.SlotNumber = GL_TEXTURE0,
			.TextureResource = texture
		});

		// create mesh for quad
		glm::mat4 transform = glm::mat4(1.0f);

		auto quadMesh = std::make_shared<OZZ::IndexVertexBuffer>();
		auto quad = OZZ::scene::SceneObject{
			.Transform = transform,
			.Mesh = quadMesh,
			.Mat = mat,
		};
		auto vertices = std::vector<OZZ::Vertex>(OZZ::quadVertices.begin(), OZZ::quadVertices.end());
		auto indices = std::vector<uint32_t>(OZZ::quadIndices.begin(), OZZ::quadIndices.end());
		quad.Mesh->UploadData(vertices, indices);

		titleScreenQuads[character] = quad;
	}
}

void MMOTitleScreen::PhysicsTick(float DeltaTime) {
	SceneLayer::PhysicsTick(DeltaTime);
}

void MMOTitleScreen::Tick(float DeltaTime) {
	SceneLayer::Tick(DeltaTime);
}

void MMOTitleScreen::RenderTargetResized(glm::ivec2 size) {
	const auto width = 1920;
	const auto height = 1080;

	LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.001f,
	                                          1000.f);
}

std::vector<OZZ::scene::SceneObject> MMOTitleScreen::GetSceneObjects() {
	auto sampleText = BuildText("Sample Text", {-256.f, -128.f, 0.f}, {1.f, 1.f, 1.f}, {1.f, 1.f, 0.f});
	auto titleText = BuildText("Lights", {0.f, (1080.f / 2) - 32.f, 0.f}, {1.f, 1.f, 1.f}, {1.f, 1.f, 0.f});

	auto titleScreenObjects = std::vector<OZZ::scene::SceneObject>();
	titleScreenObjects.reserve(sampleText.size() + titleText.size());
	titleScreenObjects.insert(titleScreenObjects.end(), sampleText.begin(), sampleText.end());
	titleScreenObjects.insert(titleScreenObjects.end(), titleText.begin(), titleText.end());
	return titleScreenObjects;
}

std::vector<OZZ::scene::SceneObject> MMOTitleScreen::BuildText(const std::string &text, const glm::vec3 &position,
	const glm::vec3 &scale, glm::vec3 color) {
	std::vector<OZZ::scene::SceneObject> textObjects;

	int nextCharacterX = position.x;
	for (auto character : text) {
		auto characterData = alphabetCharacterData[character];
		auto characterObj = titleScreenQuads[character];

		auto characterPosition = glm::vec3(
			(characterData->Size.x / 2) + (nextCharacterX + characterData->Bearing.x),
			(position.y + characterData->Bearing.y * scale.y) - (characterData->Size.y / 2),
			0.f);
		characterObj.Transform = glm::translate(characterObj.Transform, characterPosition);
		characterObj.Transform = glm::scale(characterObj.Transform, glm::vec3(scale.x * characterData->Size.x, scale.y * characterData->Size.y, 1.f));
		characterObj.Mat->GetShader()->SetVec3("textColor", color);

		nextCharacterX += characterData->Advance >> 6;

		textObjects.push_back(characterObj);
	}

	return textObjects;
}
