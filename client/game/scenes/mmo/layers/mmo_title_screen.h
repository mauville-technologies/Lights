//
// Created by ozzadar on 2025-04-06.
//

#pragma once

#include <lights/scene/scene.h>
#include <lights/text/font_loader.h>

class MMOTitleScreen : public OZZ::scene::SceneLayer {
public:
	~MMOTitleScreen() override = default;

	/// Override
	void Init() override;
	void PhysicsTick(float DeltaTime) override;
	void Tick(float DeltaTime) override;

    void RenderTargetResized(glm::ivec2 size) override;
    std::vector<OZZ::scene::SceneObject> GetSceneObjects() override;

private:
	std::vector<OZZ::scene::SceneObject> BuildText(const std::string &text, const glm::vec3 &position, const glm::vec3 &scale, glm::vec3 color = {1.f, 1.f, 1.f});

private:
	std::filesystem::path fontPath { "assets/fonts/game_bubble.ttf" };
	std::unique_ptr<OZZ::FontLoader> fontLoader { nullptr };

	std::unordered_map<char, OZZ::Character*> alphabetCharacterData {};

	// title screen material
	std::shared_ptr<OZZ::Material> titleScreenMaterial { nullptr };

	// title screen quads
	std::unordered_map<char, OZZ::scene::SceneObject> titleScreenQuads {};
};
