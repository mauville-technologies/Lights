//
// Created by ozzadar on 2025-04-06.
//

#include "mmo_scene.h"

#include <game/library/layers/ui_layer.h>

#include "layers/mmo_character_select.h"
#include "layers/mmo_title_screen.h"
#include "layers/mmo_game.h"

void MMOScene::Init(std::shared_ptr<OZZ::InputSubsystem> inInput) {
	Scene::Init(inInput);
	Params.ClearColor = glm::vec3(0.0f, 0.0f, 0.0f);

	// load the UI layer
	auto* ui = layerManager->LoadLayer<OZZ::lights::library::layers::UILayer>("UILayer", GetWorld());
	ui->SetInputSubsystem(inInput);

	auto* TitleScreen = layerManager->LoadLayer<MMOTitleScreen>("TitleScreen", GetWorld(), ui);
	TitleScreen->SetInputSubsystem(inInput);

	// we want the UI layer to always be on top
	layerManager->SetLayerActive("UILayer", true);
	layerManager->SetLayerActive("TitleScreen", true);
	layerManager->LoadLayer<MMOCharacterSelect>("CharacterSelect");
	layerManager->SetLayerActive("CharacterSelect", false);
	layerManager->LoadLayer<MMOGame>("Game");
	layerManager->SetLayerActive("Game", false);

	layerManager->Init();
}

void MMOScene::WindowResized(glm::ivec2 size) {
	// locking to 1920 x 1080
	// Scene::WindowResized({1920, 1080});
	Scene::WindowResized(size);
}
