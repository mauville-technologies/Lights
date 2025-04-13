//
// Created by ozzadar on 2025-04-06.
//

#pragma once
#include <unordered_map>
#include <lights/game/game_object.h>
#include <lights/text/font_loader.h>

namespace OZZ {
	class FontLoader;
}

namespace OZZ::game::objects {
	class TextLabel : public GameObject {
	public:

		explicit TextLabel(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld,
		                   const std::filesystem::path& inFontPath, uint16_t inFontSize = 32, const std::string& inText = "",
		                   const glm::vec3 &inColor = {1.f, 1.f, 1.f}, bool inLikelyToChange = false);

		void Tick(float DeltaTime) override;
		std::vector<scene::SceneObject> GetSceneObjects() override;

		void SetText(const std::string& string);
		void SetColor(const glm::vec3& inColor);

	private:

		void rebuildText();
		void reloadCharacterSet();
		void updateText();
		void updateCharacterTransforms();
	private:
		// Parameters
		std::unique_ptr<OZZ::FontLoader> fontLoader { nullptr };
		std::filesystem::path fontPath;
		uint16_t fontSize;
		std::string text;
		glm::vec3 color;
		// bLikelyToChange is used to determine if the text is likely to change, which affects the way textures are cached
		bool bLikelyToChange;

		std::unordered_map<char, Character*> characterSet;

		// internal junk
		FontSet* fontSet { nullptr };

		std::shared_ptr<OZZ::Texture> fontTexture;
		std::shared_ptr<OZZ::Shader> fontShader { nullptr };
		std::shared_ptr<OZZ::Material> fontMaterial { nullptr };
		std::shared_ptr<OZZ::IndexVertexBuffer> fontMesh { nullptr };

		OZZ::scene::SceneObject fontRenderObject {};

		// cache things
		glm::vec3 builtPosition;
		glm::vec3 builtScale;
		std::string builtText;
		glm::quat builtRotation;
	};
} // game
