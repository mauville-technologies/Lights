//
// Created by ozzadar on 2025-04-06.
//

#include "mmo_title_screen.h"

#include <lights/rendering/shapes.h>
#include <game/objects/text/text_label.h>

MMOTitleScreen::MMOTitleScreen(OZZ::GameWorld *inWorld) : gameWorld(inWorld) {
}

void MMOTitleScreen::Init() {
	SceneLayer::Init();
	LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), // Camera position
	                                     glm::vec3(0.f, 0.f, 0.f), // Target to look at
	                                     glm::vec3(0.f, 1.f, 0.f)); // Up vector

	titleScreenText = gameWorld->CreateGameObject<OZZ::game::objects::TextLabel>(
		std::filesystem::path("assets/fonts/game_bubble.ttf"), 128, "Lights Text Label");

}

void MMOTitleScreen::PhysicsTick(float DeltaTime) {
	SceneLayer::PhysicsTick(DeltaTime);
}

void MMOTitleScreen::Tick(float DeltaTime) {
	SceneLayer::Tick(DeltaTime);

	// TODO: I feel like the gameworld doesn't need to be owned by the layer -- though I'm not entirely sure of the alternative.
	// I'll leave it here for now and see if it makes sense to move it somewhere else (like the scene) further down the line
	// Doing it here makes it easy to forget in the future if
	gameWorld->Tick(DeltaTime);

	static glm::vec3 titleScreenOffset {-400.f, 0.f, 0.f};
	static float totalTime = 1.f;

	totalTime += DeltaTime;
	// apply a sin wave
	titleScreenOffset.y = std::sin(totalTime * 2.f) * 64;

	titleScreenText.second->SetPosition(titleScreenOffset);

}

void MMOTitleScreen::RenderTargetResized(glm::ivec2 size) {
	const auto width = 1920;
	const auto height = 1080;

	LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.001f,
	                                          1000.f);
}

std::vector<OZZ::scene::SceneObject> MMOTitleScreen::GetSceneObjects() {
	auto titleTextObjObjects = titleScreenText.second->GetSceneObjects();

	auto titleScreenObjects = std::vector<OZZ::scene::SceneObject>();
	titleScreenObjects.insert(titleScreenObjects.end(), titleTextObjObjects.begin(), titleTextObjObjects.end());
	return titleScreenObjects;
}