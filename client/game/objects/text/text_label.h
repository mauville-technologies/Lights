//
// Created by ozzadar on 2025-04-06.
//

#pragma once
#include <lights/game/game_object.h>
#include <lights/text/font_loader.h>
#include <lights/game/anchors.h>

namespace OZZ {
	class FontLoader;
}

namespace OZZ::game::objects {
	class TextLabel : public GameObject {
	public:

		explicit TextLabel(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld,
		                   const std::filesystem::path& inFontPath, uint16_t inFontSize = 32, const std::string& inText = "",
		                   const glm::vec3 &inColor = {1.f, 1.f, 1.f}, AnchorPoint inAnchorPoint = AnchorPoint::CenterMiddle);

		void Tick(float DeltaTime) override;
		std::vector<scene::SceneObject> GetSceneObjects() override;

		void SetText(const std::string& string);
		void SetColor(const glm::vec3& inColor);
		void SetRectBounds(const glm::vec2& size);

		[[nodiscard]] glm::vec2 GetCharacterSize() const {
			return characterSize;
		};
	private:

		void rebuildText();
		void reloadCharacterSet();
		void updateText();
		void updateTransform();
		void updateRectBounds();

		[[nodiscard]] glm::vec2 getTotalSize() const;
		[[nodiscard]] glm::vec2 getTopLeftVertexPosition() const;
		[[nodiscard]] glm::vec2 getAnchorPosition() const;
	private:
		// Parameters
		std::unique_ptr<OZZ::FontLoader> fontLoader { nullptr };
		std::filesystem::path fontPath;
		uint16_t fontSize;
		std::string text;
		glm::vec3 color;
		AnchorPoint anchorPoint;
		glm::vec2 rectBounds {0.f};

		// internal junk
		FontSet* fontSet { nullptr };
		glm::vec2 characterSize {};

		std::shared_ptr<OZZ::Texture> fontTexture;
		std::shared_ptr<OZZ::Shader> fontShader { nullptr };
		std::shared_ptr<OZZ::Material> fontMaterial { nullptr };
		std::shared_ptr<OZZ::IndexVertexBuffer> fontMesh { nullptr };
		OZZ::scene::SceneObject fontRenderObject {};

		// cache things
		bool bBuilt {false};
		glm::vec3 builtPosition {};
		glm::vec3 builtScale {};
		std::string builtText {};
		glm::quat builtRotation {};
		glm::vec2 builtRectBounds {};
	};
} // game
