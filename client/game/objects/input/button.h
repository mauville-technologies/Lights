//
// Created by ozzadar on 2025-04-13.
//

#pragma once
#include <lights/game/game_object.h>
#include <lights/input/input_subsystem.h>

#include <game/objects/text/text_label.h>

namespace OZZ::game::objects {
	class Button : public GameObject {
	public:
		struct ButtonParams {
			uint16_t FontSize { 32 };
			std::filesystem::path FontPath {""};
			std::string Text;

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
		};

        explicit Button(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld, const ButtonParams &inParams = {});
		~Button() override;
        void Tick(float DeltaTime) override;
        std::vector<scene::SceneObject> GetSceneObjects() override;

		void SetupInput(InputSubsystem* inInputSubsystem);
		void SetFocused(bool focused);

	protected:
		void onPositionChanged() override;

	private:
		void setText(const std::string &inText);
		void updateTextLabel() const;

	private:
		std::pair<uint64_t, TextLabel*> label {UINT16_MAX, nullptr};
		InputSubsystem* inputSubsystem { nullptr };
		std::string inputSID {};

		std::string currentString {};
		ButtonParams params {};

        scene::SceneObject backgroundBox {};
		scene::SceneObject cursor {};
		bool isFocused { false };
	};
}
