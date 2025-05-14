//
// Created by ozzadar on 2025-04-06.
//

#include "mmo_character_select.h"

void MMOCharacterSelect::Init() {
	SceneLayer::Init();
}

void MMOCharacterSelect::PhysicsTick(float DeltaTime) {
	SceneLayer::PhysicsTick(DeltaTime);
}

void MMOCharacterSelect::Tick(float DeltaTime) {
	SceneLayer::Tick(DeltaTime);
}

void MMOCharacterSelect::RenderTargetResized(glm::ivec2 size) {
}

std::vector<OZZ::scene::SceneObject> MMOCharacterSelect::GetSceneObjects() {
	return {};
}
