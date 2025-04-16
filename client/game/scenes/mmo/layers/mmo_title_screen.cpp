//
// Created by ozzadar on 2025-04-06.
//

#include "mmo_title_screen.h"

#include <game/objects/text/text_input.h>
#include <lights/rendering/shapes.h>
#include <game/objects/text/text_label.h>

MMOTitleScreen::MMOTitleScreen(OZZ::GameWorld *inWorld) : gameWorld(inWorld) {
}

void MMOTitleScreen::Init() {
	SceneLayer::Init();
	LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), // Camera position
	                                     glm::vec3(0.f, 0.f, 0.f), // Target to look at
	                                     glm::vec3(0.f, 1.f, 0.f)); // Up vector

	OZZ::game::objects::TextInput::TextInputParams textInputParams {};
	textInputParams.FontPath = "assets/fonts/game_bubble.ttf";
	textInputParams.FontSize = 32;
	textInputParams.Size = {400.f, 100.f};
	textInputParams.BackgroundColor = {1.0f, 0.2f, 0.2f, 0.5f};
	textInputParams.TextAnchorPoint = AnchorPoint::LeftMiddle;
	// bright blue border color
	textInputParams.FocusedColor = {1.f, 0.2f, 1.f, 1.f};
	textInputParams.FocusedThickness = glm::vec4{5.f};

	inputBoxes.emplace_back(gameWorld->CreateGameObject<OZZ::game::objects::TextInput>(textInputParams));
	inputBoxes.back().second->SetupInput(input.get());
	inputBoxes.back().second->SetFocused(true);
	inputBoxes.back().second->SetPosition({-50.f, 100.f, 0.f});

	textInputParams.bIsPassword = true;
	textInputParams.FontSize = 64;
	textInputParams.TextAnchorPoint = AnchorPoint::LeftMiddle;
	inputBoxes.emplace_back(gameWorld->CreateGameObject<OZZ::game::objects::TextInput>(textInputParams));
	inputBoxes.back().second->SetupInput(input.get());
	inputBoxes.back().second->SetFocused(false);
	inputBoxes.back().second->SetPosition({-50.f, -100.f, 0.f});
	focusedBox = 0;
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

	// titleScreenText.second->SetPosition(titleScreenOffset);

}

void MMOTitleScreen::SetInputSubsystem(const std::shared_ptr<OZZ::InputSubsystem> &inInput) {
	input = inInput;

	// listen to TAB to switch input boxes
	input->RegisterInputMapping(OZZ::InputMapping{
		.Action = "SelectNextInputBox",
		.Chords = {OZZ::InputChord{.Keys = std::vector<OZZ::InputKey>{{-1, OZZ::EKey::Tab}}}},
		.Callbacks = {
			.OnPressed = [this] () {
				selectNextInputBox();
			},
		}
	});
}

void MMOTitleScreen::RenderTargetResized(glm::ivec2 size) {
	const auto width = 1920;
	const auto height = 1080;

	LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.001f,
	                                          1000.f);
}

std::vector<OZZ::scene::SceneObject> MMOTitleScreen::GetSceneObjects() {
	// auto titleTextObjObjects = titleScreenText.second->GetSceneObjects();

	auto titleScreenObjects = std::vector<OZZ::scene::SceneObject>();
	for (const auto inputbox : inputBoxes) {
		auto sceneObjects = inputbox.second->GetSceneObjects();
		titleScreenObjects = titleScreenObjects + sceneObjects;
	}
	return titleScreenObjects;
}

void MMOTitleScreen::selectNextInputBox() {
	// delesect the current box
	if (focusedBox < inputBoxes.size()) {
		inputBoxes[focusedBox].second->SetFocused(false);
	}
	focusedBox++;

	// if we're at the end of the list plus one, no selection
	if (focusedBox > inputBoxes.size()) {
		focusedBox = 0;
	}

	spdlog::info("Selected box: {}", focusedBox);
	if (focusedBox < inputBoxes.size()) {
		inputBoxes[focusedBox].second->SetFocused(true);
	}
}
