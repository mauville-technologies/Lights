//
// Created by ozzadar on 2025-04-06.
//

#include "text_label.h"

#include <lights/rendering/shapes.h>
#include <lights/text/font_loader.h>

#include <utility>

namespace OZZ::game::objects {
	TextLabel::TextLabel(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld,
	                     std::filesystem::path inFontPath, const uint16_t inFontSize,
	                     std::string inText, const glm::vec3 &inColor, const AnchorPoint inAnchorPoint)
		: GameObject(inGameWorld, std::move(inPhysicsWorld)),
		  fontLoader(std::make_unique<OZZ::FontLoader>()), fontPath(std::move(inFontPath)), fontSize(inFontSize),
			text(std::move(inText)), color(inColor), anchorPoint(inAnchorPoint){

		rebuildText();
		TextLabel::onPositionChanged();
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

	void TextLabel::onPositionChanged() {
		GameObject::onPositionChanged();
		updateTransform();
	}

	void TextLabel::onScaleChanged() {
		GameObject::onScaleChanged();
		updateTransform();
;	}

	void TextLabel::onRotationChanged() {
		GameObject::onRotationChanged();
		updateTransform();
	}

	void TextLabel::onParentChanged() {
		GameObject::onParentChanged();
		updateTransform();
	}

	void TextLabel::rebuildText() {
		reloadCharacterSet();
		updateText();
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

		builtText = "";
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

			auto topLeftPosition = getTopLeftVertexPosition();

			auto [UV, Size, Bearing, Advance] = fontSet->Characters[character];
			const auto& scale = GetScale();
			auto characterTopLeft = glm::vec3(
				topLeftPosition.x + (Bearing.x + nextCharacterX * scale.x),
				topLeftPosition.y -(Size.y - Bearing.y) * scale.y,
				0.f);
			// first vertex
			auto topLeft = Vertex{
				.position = characterTopLeft,
				.color = {1.f, 1.f, 1.f, 1.f},
				.uv = {UV.x, UV.y},
			};
			// second vertex
			auto topRight = Vertex{
				.position = characterTopLeft + glm::vec3(Size.x * scale.x, 0.f, 0.f),
				.color = {1.f, 1.f, 1.f, 1.f},
				.uv = {UV.z, UV.y},
			};
			// third vertex
			auto bottomLeft = Vertex{
				.position = characterTopLeft + glm::vec3(0.f, Size.y * scale.y, 0.f),
				.color = {1.f, 1.f, 1.f, 1.f},
				.uv = {UV.x, UV.w},
			};
			// fourth vertex
			auto bottomRight = Vertex{
				.position = characterTopLeft + glm::vec3(Size.x * scale.x, Size.y * scale.y, 0.f),
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

		const auto& position = GetPosition();
		fontRenderObject = {
			.Transform = glm::translate(glm::mat4{1.f}, position),
			.Mesh = fontMesh,
			.Mat = fontMaterial,
		};

		updateTransform();
		builtText = text;
	}

	void TextLabel::updateTransform() {

		fontRenderObject.Transform = GetWorldTransform();
		// const auto& position = GetPosition();
		// const auto& scale = GetScale();
		// const auto& rotation = GetRotation();
		//
		// const auto parentOffset = parent? parent->GetPosition() : glm::vec3{0.f};
		// spdlog::info("|\t {} \t| parent: {} {} {}", text, parentOffset.x, parentOffset.y, parentOffset.z);
		// // TODO:: ADD TRANSFORM TO THE GAMEOBJECT, FOLLOW PARENT CHAIN
		// fontRenderObject.Transform = glm::translate(glm::mat4{1.f}, parentOffset + position + glm::vec3(getAnchorPosition(), 1));
		// const auto parentRotation = parent? parent->GetRotation() : glm::quat{};
		// fontRenderObject.Transform *= glm::mat4_cast(glm::normalize(parentRotation * rotation));
		// const auto parentScale = parent? parent->GetScale() : glm::vec3{1.f};
		// fontRenderObject.Transform = glm::scale(fontRenderObject.Transform, parentScale * scale);

		builtPosition = GetPosition();
		builtScale = GetPosition();
		builtRotation = GetRotation();
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
		auto leftBounds = anchorPosition.x;
		auto rightBounds = anchorPosition.x;
		auto topBounds = anchorPosition.y;
		auto bottomBounds = anchorPosition.y;

		const bool bIsCenter = anchorPoint == AnchorPoint::CenterTop ||
			anchorPoint == AnchorPoint::CenterBottom || anchorPoint == AnchorPoint::CenterMiddle;
		const bool bIsLeft = anchorPoint == AnchorPoint::LeftTop ||
			anchorPoint == AnchorPoint::LeftBottom || anchorPoint == AnchorPoint::LeftMiddle;
		const bool bIsRight = anchorPoint == AnchorPoint::RightTop ||
			anchorPoint == AnchorPoint::RightBottom || anchorPoint == AnchorPoint::RightMiddle;

		if (bIsCenter) {
			leftBounds = -rectBounds.x / 2;
			rightBounds = rectBounds.x / 2;
		}
		if (bIsLeft) {
			leftBounds = 0;
			rightBounds += rectBounds.x;
		}
		if (bIsRight) {
			leftBounds -= rectBounds.x;
		}

		topBounds = -rectBounds.y / 2;
		bottomBounds = rectBounds.y / 2;

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

	glm::vec2 TextLabel::getTopLeftVertexPosition() const {
		const auto size = getTotalSize();
		const auto totalWidth = size.x;
		const auto totalHeight = size.y;
		glm::vec2 topLeftPosition = {0.f, 0.f};
		switch (anchorPoint) {
			case AnchorPoint::LeftTop:
			case AnchorPoint::LeftBottom:
			case AnchorPoint::LeftMiddle:
				topLeftPosition.x = 0;
				break;
			case AnchorPoint::RightTop:
			case AnchorPoint::RightBottom:
			case AnchorPoint::RightMiddle:
				spdlog::error("Right align not implemented in text label yet");
				break;
			case AnchorPoint::CenterTop:
			case AnchorPoint::CenterMiddle:
			case AnchorPoint::CenterBottom:
				topLeftPosition.x = -(totalWidth / 2);
				break;
		}
		topLeftPosition.y = -(totalHeight / 2);
		return topLeftPosition;
	}

	glm::vec2 TextLabel::getAnchorPosition() const {
		glm::vec2 anchorPosition = {0.f, 0.f};

		switch (anchorPoint) {
			case AnchorPoint::LeftTop:
			case AnchorPoint::LeftBottom:
			case AnchorPoint::LeftMiddle:
				anchorPosition.x = 0;
				break;
			case AnchorPoint::RightTop:
			case AnchorPoint::RightBottom:
			case AnchorPoint::RightMiddle:
				anchorPosition.x = rectBounds.x / 2;
				break;
			case AnchorPoint::CenterMiddle:
			case AnchorPoint::CenterTop:
			case AnchorPoint::CenterBottom:
				break;
		}

		switch (anchorPoint) {
			case AnchorPoint::LeftBottom:
			case AnchorPoint::RightBottom:
			case AnchorPoint::CenterBottom:
				anchorPosition.y = -rectBounds.y + fontSet->CharacterSize.y;
				break;
			case AnchorPoint::LeftTop:
			case AnchorPoint::RightTop:
			case AnchorPoint::CenterTop:
				anchorPosition.y = rectBounds.y -fontSet->CharacterSize.y /2;
				break;
			case AnchorPoint::LeftMiddle:
			case AnchorPoint::RightMiddle:
			case AnchorPoint::CenterMiddle:
				break;
		}
		return anchorPosition;
	}
}
