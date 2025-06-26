//
// Created by ozzadar on 2025-04-06.
//

#pragma once
#include <lights/library/objects/ui/ui_component.h>
#include <lights/text/font_loader.h>
#include <lights/game/anchors.h>

namespace OZZ {
	class FontLoader;
}

namespace OZZ::game::objects {
	class TextLabel : public UIComponent {
	public:
		struct TextLabelParams {
			std::filesystem::path  FontPath;
			uint16_t FontSize = 32;
			std::string Text;
			glm::vec3 Color = {1.f, 1.f, 1.f};
			AnchorPoint AnchorPoint = AnchorPoint::CenterMiddle;
		};

		using ParamsType = TextLabelParams;

		explicit TextLabel(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld, const TextLabelParams &inParams = {});

		void Tick(float DeltaTime) override;
		std::vector<scene::SceneObject> GetSceneObjects() override;

		void SetText(const std::string& string);
		void SetColor(const glm::vec3& inColor);
		void SetRectBounds(const glm::vec2& size);

		[[nodiscard]] glm::vec2 GetCharacterSize() const {
			return characterSize;
		};

	protected:
		void onPositionChanged() override;
		void onScaleChanged() override;
		void onRotationChanged() override;
		void onParentChanged() override;
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
		ParamsType params;

		std::unique_ptr<OZZ::FontLoader> fontLoader { nullptr };
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
