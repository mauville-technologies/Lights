//
// Created by ozzadar on 2025-04-06.
//

#include "text_label.h"

#include <lights/rendering/shapes.h>
#include <lights/text/font_loader.h>

namespace OZZ::game::objects {
	TextLabel::TextLabel(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld,
	                     const std::filesystem::path &inFontPath, const uint16_t inFontSize,
	                     const std::string &inText, const glm::vec3 &inColor, AnchorPoint inAnchorPoint)
		: GameObject(inGameWorld, std::move(inPhysicsWorld)),
		  fontLoader(std::make_unique<OZZ::FontLoader>()), fontPath(inFontPath), fontSize(inFontSize),
			text(inText), color(inColor), anchorPoint(inAnchorPoint){

		rebuildText();
	}

	void TextLabel::Tick(float DeltaTime) {
		rebuildText();

		// static int frameCount = 0;
		// frameCount++;
		//
		// // rotate a little
		// if (frameCount % 1000 == 0) {
		// 	// rotate a little
		// 	// create 1 degree quaternion
		// 	Rotation = glm::normalize(Rotation * glm::angleAxis(glm::radians(-1.f), glm::vec3(0.f, 0.f, 1.f)));
		// }
	}

	std::vector<scene::SceneObject> TextLabel::GetSceneObjects() {
		return {fontRenderObject};
	}

	void TextLabel::SetText(const std::string &string) {
		text = string;
		rebuildText();
	}

	void TextLabel::SetColor(const glm::vec3 &inColor) {
		if (inColor != color) {
			color = inColor;
			rebuildText();
		}
	}

	void TextLabel::SetRectBounds(const glm::vec2 &size) {
		rectBounds = size;
	}

	void TextLabel::rebuildText() {
		reloadCharacterSet();
		updateText();
		updateTransform();
		updateRectBounds();
		bBuilt = true;
	}

	void TextLabel::reloadCharacterSet() {
		if (!fontSet) {
			fontSet = fontLoader->GetFontSet(fontPath, fontSize);
			characterSize = fontSet->CharacterSize;
		}
	}

	void TextLabel::updateText() {
		if (!fontSet) {
			spdlog::error("FontSet is null");
			return;
		}

		if ((text == builtText && bBuilt)) return;
		fontRenderObject = {};
		// create the texture
		fontTexture.reset();
		fontTexture = std::make_shared<OZZ::Texture>();
		fontTexture->UploadData(fontSet->Texture.get());

		// crate the shader
		fontShader = std::make_shared<OZZ::Shader>("assets/shaders/text/font.vert", "assets/shaders/text/font.frag");

		//create the material
		fontMaterial = std::make_shared<OZZ::Material>();
		fontMaterial->SetShader(fontShader);
		fontMaterial->AddTextureMapping({
			.SlotName = "inTexture",
			.SlotNumber = GL_TEXTURE0,
			.TextureResource = fontTexture,
		});
		fontMaterial->AddUniformSetting({
			.Name = "textColor",
			.Value = color,
		});


		// create the mesh
		auto meshVertices = std::vector<Vertex>();
		auto meshIndices = std::vector<uint32_t>();

		// build the text object
		int nextCharacterX = 0;
		int startIndex = 0;

		for (auto& character : text) {
			// get index of the character
			auto characterIndex = std::string(FontLoader::CharacterSet).find(character);
			if (characterIndex == std::string::npos) {
				spdlog::error("Character {} not found in character set", character);
				continue;
			}

			auto anchorPosition = getAnchorPosition();

			auto [UV, Size, Bearing, Advance] = fontSet->Characters[character];
			auto characterTopLeft = glm::vec3(
				anchorPosition.x + (Bearing.x + nextCharacterX * Scale.x),
				anchorPosition.y -(Size.y - Bearing.y) * Scale.y,
				0.f);
			// first vertex
			auto topLeft = Vertex{
				.position = characterTopLeft,
				.color = {1.f, 1.f, 1.f, 1.f},
				.uv = {UV.x, UV.y},
			};
			// second vertex
			auto topRight = Vertex{
				.position = characterTopLeft + glm::vec3(Size.x * Scale.x, 0.f, 0.f),
				.color = {1.f, 1.f, 1.f, 1.f},
				.uv = {UV.z, UV.y},
			};
			// third vertex
			auto bottomLeft = Vertex{
				.position = characterTopLeft + glm::vec3(0.f, Size.y * Scale.y, 0.f),
				.color = {1.f, 1.f, 1.f, 1.f},
				.uv = {UV.x, UV.w},
			};
			// fourth vertex
			auto bottomRight = Vertex{
				.position = characterTopLeft + glm::vec3(Size.x * Scale.x, Size.y * Scale.y, 0.f),
				.color = {1.f, 1.f, 1.f, 1.f},
				.uv = {UV.z, UV.w},
			};

			meshVertices.push_back(topLeft);
			meshVertices.push_back(topRight);
			meshVertices.push_back(bottomLeft);
			meshVertices.push_back(bottomRight);
			meshIndices.push_back(startIndex + 0);
			meshIndices.push_back(startIndex + 1);
			meshIndices.push_back(startIndex + 2);
			meshIndices.push_back(startIndex + 1);
			meshIndices.push_back(startIndex + 3);
			meshIndices.push_back(startIndex + 2);

			startIndex += 4;
			nextCharacterX += Advance.x >> 6;
		}
		fontMesh = std::make_shared<OZZ::IndexVertexBuffer>();
		fontMesh->UploadData(meshVertices, meshIndices);

		fontRenderObject = {
			.Transform = glm::translate(glm::mat4{1.f}, Position),
			.Mesh = fontMesh,
			.Mat = fontMaterial,
		};

		builtText = text;
	}

	void TextLabel::updateTransform() {
		if (text.empty() || !bBuilt || (builtPosition == Position && builtScale == Scale && builtRotation == Rotation)) return;

		fontRenderObject.Transform = glm::translate(glm::mat4{1.f}, Position);
		fontRenderObject.Transform *= glm::mat4_cast(Rotation);
		fontRenderObject.Transform = glm::scale(fontRenderObject.Transform, Scale);

		builtPosition = Position;
		builtScale = Scale;
		builtRotation = Rotation;
	}

	void TextLabel::updateRectBounds() {
		if (builtRectBounds == glm::vec2{0.f} && rectBounds != glm::vec2{0.f}) {
			// remove the uniform
			fontMaterial->RemoveUniformSetting("rectBounds");
			builtRectBounds = rectBounds;
			return;
		}

		if (builtRectBounds == glm::vec2{0.f}) return;

		// TODO: I also need to take position into account
		const auto anchorPosition = getAnchorPosition();
		const auto leftBounds = anchorPosition.x;
		const auto rightBounds = anchorPosition.x + rectBounds.x;
		const auto topBounds = anchorPosition.y;
		const auto bottomBounds = anchorPosition.y + rectBounds.y;
		fontMaterial->AddUniformSetting({
			.Name = "rectBounds",
            .Value = glm::vec4{
                leftBounds, topBounds,
            	rightBounds, bottomBounds,
            },
		});
		builtRectBounds = rectBounds;
	}

	glm::vec2 TextLabel::getTotalSize() const {
		int totalWidth = 0;
		int totalHeight = 0;
		for (auto& character : text) {
			auto characterIndex = std::string(FontLoader::CharacterSet).find(character);
			if (characterIndex == std::string::npos) {
				spdlog::error("Character {} not found in character set", character);
				continue;
			}
			auto [UV, Size, Bearing, Advance] = fontSet->Characters[character];
			// calculate the total width
			totalWidth += Advance.x >> 6;
			totalHeight = std::max(totalHeight, Size.y);
		}
		return {totalWidth, totalHeight};
	}

	glm::vec2 TextLabel::getAnchorPosition() const {
		const auto size = getTotalSize();
		const auto totalWidth = size.x;
		const auto totalHeight = size.y;
		glm::vec2 anchorPosition = {0.f, 0.f};
		switch (anchorPoint) {
			case AnchorPoint::CenterLeft:
                anchorPosition = {0.f, -(totalHeight / 2)};
                break;
			case AnchorPoint::Center:
			default:
                anchorPosition = {-(totalWidth / 2), -(totalHeight / 2)};
                break;
		}
		return anchorPosition;
	}
}
