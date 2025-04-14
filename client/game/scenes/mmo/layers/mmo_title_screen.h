//
// Created by ozzadar on 2025-04-06.
//

#pragma once

#include <lights/scene/scene.h>
#include <lights/text/font_loader.h>

namespace OZZ::game::objects {
	class TextInput;
	class TextLabel;
}

class MMOTitleScreen : public OZZ::scene::SceneLayer {
public:
	MMOTitleScreen(OZZ::GameWorld* inWorld);
	~MMOTitleScreen() override = default;

	/// Override
	void Init() override;
	void PhysicsTick(float DeltaTime) override;
	void Tick(float DeltaTime) override;
	void SetInputSubsystem(const std::shared_ptr<OZZ::InputSubsystem>& inInput);
    void RenderTargetResized(glm::ivec2 size) override;
    std::vector<OZZ::scene::SceneObject> GetSceneObjects() override;

private:
	std::filesystem::path fontPath { "assets/fonts/game_bubble.ttf" };
	std::unique_ptr<OZZ::FontLoader> fontLoader { nullptr };

	// Keep the things under here
	OZZ::GameWorld* gameWorld;
	std::pair<uint64_t, OZZ::game::objects::TextLabel*> titleScreenText { UINT64_MAX, nullptr };
	std::pair<uint64_t, OZZ::game::objects::TextInput*> UsernameInputBox { UINT64_MAX, nullptr };


	std::shared_ptr<OZZ::InputSubsystem> input;
};

