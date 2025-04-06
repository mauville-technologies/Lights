//
// Created by ozzadar on 2025-04-06.
//

#include "mmo_game.h"

void MMOGame::Init() {
	SceneLayer::Init();
}

void MMOGame::PhysicsTick(float DeltaTime) {
	SceneLayer::PhysicsTick(DeltaTime);
}

void MMOGame::Tick(float DeltaTime) {
	SceneLayer::Tick(DeltaTime);
}

void MMOGame::RenderTargetResized(glm::ivec2 size) {
}

std::vector<OZZ::scene::SceneObject> MMOGame::GetSceneObjects() {
	return {};
}
