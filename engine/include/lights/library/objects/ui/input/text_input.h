//
// Created by ozzadar on 2025-04-13.
//

#pragma once
#include <lights/library/objects/ui/ui_component.h>
#include <lights/input/input_subsystem.h>

#include <lights/library/objects/ui/text/text_label.h>

namespace OZZ::game::objects {
	class TextInput : public UIComponent {
	public:
		struct TextInputParams {
			uint16_t FontSize {32};
			std::filesystem::path FontPath {""};

			glm::vec2 Size {
				200.f, 50.f
			};

			glm::vec4 BackgroundColor {
				0.2f, 0.2f, 0.2f, 1.f
			};

			glm::vec3 CursorColor {
				1.f, 1.f, 1.f
			};

			glm::vec3 TextColor {
				1.f, 1.f, 0.f
			};

			glm::vec4 FocusedColor {
				1.f, 0.f, 0.5f, 1.f
			};

			glm::vec4 FocusedThickness {5.f};

			AnchorPoint TextAnchorPoint {AnchorPoint::LeftMiddle};

			bool bIsPassword {false};

			TextInputParams() :
				FontSize(32),
				FontPath(""),
				Size(200.f, 50.f),
				BackgroundColor(0.2f, 0.2f, 0.2f, 1.f),
				CursorColor(1.f, 1.f, 1.f),
				TextColor(1.f, 1.f, 0.f),
				FocusedColor(1.f, 0.f, 0.5f, 1.f),
				FocusedThickness(5.f),
				TextAnchorPoint(AnchorPoint::LeftMiddle),
				bIsPassword(false) {}
		};

		using ParamsType = TextInputParams;
        explicit TextInput(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld, const TextInputParams &inParams = {});
		~TextInput() override;
        void Tick(float DeltaTime) override;
        std::vector<scene::SceneObject> GetSceneObjects() override;

		void SetupInput(InputSubsystem* inInputSubsystem);

		bool IsMouseOver(const glm::vec2 &worldPos) override;
        void Clicked() override;

	protected:
		void onFocusChanged() override;
		void onPositionChanged() override;

	private:
		void updateTextLabel() const;
		void appendCharacter(char character);
		void removeCharacter();

	private:
		std::pair<uint64_t, TextLabel*> label {UINT16_MAX, nullptr};
		InputSubsystem* inputSubsystem { nullptr };
		std::string inputSID {};

		std::string currentString {};
		TextInputParams params {};

        scene::SceneObject backgroundBox {};
		scene::SceneObject cursor {};
	};
}
