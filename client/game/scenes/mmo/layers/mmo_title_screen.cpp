//
// Created by ozzadar on 2025-04-06.
//

#include "mmo_title_screen.h"

void MMOTitleScreen::Init() {
	SceneLayer::Init();
}

void MMOTitleScreen::PhysicsTick(float DeltaTime) {
	SceneLayer::PhysicsTick(DeltaTime);
}

void MMOTitleScreen::Tick(float DeltaTime) {
	SceneLayer::Tick(DeltaTime);
}

void MMOTitleScreen::RenderTargetResized(glm::ivec2 size) {
}

std::vector<OZZ::scene::SceneObject> MMOTitleScreen::GetSceneObjects() {
	return {};
}
