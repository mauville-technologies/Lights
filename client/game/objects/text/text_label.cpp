//
// Created by ozzadar on 2025-04-06.
//

#include "text_label.h"

#include <lights/rendering/shapes.h>
#include <lights/text/font_loader.h>

namespace OZZ::game::objects {
	TextLabel::TextLabel(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld,
	                     const std::filesystem::path &inFontPath, const uint16_t inFontSize,
	                     const std::string &inText, const glm::vec3 &inColor, const bool inLikelyToChange)
		: GameObject(inGameWorld, std::move(inPhysicsWorld)),
		  fontLoader(std::make_unique<OZZ::FontLoader>()), fontPath(inFontPath), fontSize(inFontSize),
			text(inText), color(inColor), bLikelyToChange(inLikelyToChange) {

		rebuildText();
	}

	void TextLabel::Tick(float DeltaTime) {
		rebuildText();
	}

	std::vector<scene::SceneObject> TextLabel::GetSceneObjects() {
		return textObjects;
	}

	void TextLabel::SetColor(const glm::vec3 &inColor) {
		if (inColor != color) {
			color = inColor;
			rebuildText();
		}
	}

	void TextLabel::rebuildText() {
		reloadCharacterSet();
		updateText();
		updateCharacterTransforms();
	}

	void TextLabel::reloadCharacterSet() {
		// we only load the character set once if it's likely to change
		if (bLikelyToChange && !characterSet.empty()) {
			return;
		}

		characterSet.clear();
		characterSet = bLikelyToChange ? fontLoader->GetCharacterSet(fontPath, fontSize) :
			fontLoader->GetString(fontPath, text, fontSize);

		// load the textures and build materials and junk
		textShader = std::make_shared<OZZ::Shader>("assets/shaders/font.vert", "assets/shaders/font.frag");
		textMaterial = std::make_shared<OZZ::Material>();
		textMaterial->SetShader(textShader);
		textMaterial->AddUniformSetting(OZZ::Material::UniformSetting{
			.Name = "textColor",
			.Value = glm::vec3{1.f, 1.f, 1.f},
		});

		textVertexBuffer = std::make_shared<OZZ::IndexVertexBuffer>();
		auto vertices = std::vector<OZZ::Vertex>(OZZ::quadVertices.begin(), OZZ::quadVertices.end());
		auto indices = std::vector<uint32_t>(OZZ::quadIndices.begin(), OZZ::quadIndices.end());
		textVertexBuffer->UploadData(vertices, indices);
	}

	void TextLabel::updateText() {
		if (text == builtText) return;

		textObjects.clear();
		for (auto& character : text) {
			auto characterData = characterSet[character];
			const auto texture = std::make_shared<OZZ::Texture>();
			texture->UploadData(characterData->Texture.get());
			const auto mat = std::make_shared<OZZ::Material>();
			mat->SetShader(textShader);
			mat->AddTextureMapping({
				.SlotName = "inTexture",
				.SlotNumber = GL_TEXTURE0,
				.TextureResource = texture
			});
			mat->AddUniformSetting(OZZ::Material::UniformSetting{
				.Name = "textColor",
				.Value = color,
			});

			textObjects.emplace_back(OZZ::scene::SceneObject {
				.Mesh = textVertexBuffer,
				.Mat = mat,
			});
		}
		builtText = text;
	}

	void TextLabel::updateCharacterTransforms() {
		if (text.empty() || (builtPosition == Position && builtScale == Scale)) return;

		// TODO: Determine width of the string, and set the position of the first character
		int nextCharacterX = Position.x;
		int index = 0;
		for (auto& character : text) {
			auto characterData = characterSet[character];
			// get the character data
			auto characterPosition = glm::vec3(
				(characterData->Size.x / 2) + (nextCharacterX + characterData->Bearing.x * Scale.x),
				(Position.y + characterData->Bearing.y * Scale.y) - (characterData->Size.y / 2),
				0.f);
			auto transform = glm::translate(glm::mat4{1.f}, characterPosition);
			transform = glm::scale(transform, glm::vec3(Scale.x * characterData->Size.x, Scale.y * characterData->Size.y, 1.f));

			textObjects[index].Transform = transform;
			nextCharacterX += characterData->Advance >> 6;
			index++;
		}

		builtPosition = Position;
		builtScale = Scale;
	}
}
