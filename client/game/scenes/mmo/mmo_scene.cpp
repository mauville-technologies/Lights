//
// Created by ozzadar on 2025-04-06.
//

#include "mmo_scene.h"

#include "layers/mmo_character_select.h"
#include "layers/mmo_title_screen.h"
#include "layers/mmo_game.h"

void MMOScene::Init(std::shared_ptr<OZZ::InputSubsystem> inInput, std::shared_ptr<OZZ::UserInterface> inUI) {
	Scene::Init(inInput, inUI);
	Params.ClearColor = glm::vec3(0.0f, 0.0f, 0.0f);

	auto* TitleScreen = layerManager->LoadLayer<MMOTitleScreen>("TitleScreen", GetWorld());
	TitleScreen->SetInputSubsystem(inInput);
	layerManager->SetLayerActive("TitleScreen", true);
	layerManager->LoadLayer<MMOCharacterSelect>("CharacterSelect");
	layerManager->SetLayerActive("CharacterSelect", false);
	layerManager->LoadLayer<MMOGame>("Game");
	layerManager->SetLayerActive("Game", false);

	layerManager->Init();
}
