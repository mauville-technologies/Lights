//
// Created by ozzadar on 2025-04-06.
//

#include "mmo_title_screen.h"

#include <ranges>
#include <game/objects/input/button.h>
#include <game/objects/input/text_input.h>
#include <lights/rendering/shapes.h>
#include <game/objects/text/text_label.h>
#include <game/utils/mouse_utils.h>

MMOTitleScreen::MMOTitleScreen(OZZ::GameWorld *inWorld) : gameWorld(inWorld) {
}

void MMOTitleScreen::SetInputSubsystem(const std::shared_ptr<OZZ::InputSubsystem> &inInput) {
	input = inInput;

	// listen to TAB to switch input boxes
	input->RegisterInputMapping(OZZ::InputMapping{
		.Action = "SelectNextInputBox",
		.Chords = {OZZ::InputChord{.Keys = std::vector<OZZ::InputKey>{{OZZ::EDeviceID::Keyboard, OZZ::EKey::Tab}}}},
		.Callbacks = {
			.OnPressed = [this]() {
				selectNextInputBox(bShiftPressed ? -1 : 1);
			},
		}
	});

	input->RegisterInputMapping(OZZ::InputMapping{
		.Action = "PreviousInputBox",
		.Chords = {
			OZZ::InputChord{.Keys = std::vector<OZZ::InputKey>{{OZZ::EDeviceID::Keyboard, OZZ::EKey::LShift}}},
			OZZ::InputChord{.Keys = std::vector<OZZ::InputKey>{{OZZ::EDeviceID::Keyboard, OZZ::EKey::RShift}}}
		},
		.Callbacks = {
			.OnPressed = [this]() {
				bShiftPressed = true;
			},
			.OnReleased = [this]() {
				bShiftPressed = false;
			}
		}
	});

	// let's look for mouse input
	input->RegisterInputMapping(OZZ::InputMapping{
		.Action = "MouseInput",
		.Chords = {OZZ::InputChord{.Keys = {{OZZ::EDeviceID::Mouse, OZZ::EMouseButton::Left}}}},
		.Callbacks = {
			.OnPressed = [this]() {
				const auto mousePosition = CenteredMousePosition(input->GetMousePosition(), {width, height});
				const auto worldPosition = ScreenToWorldPosition(input->GetMousePosition(), {width, height}, 
					LayerCamera.ProjectionMatrix, LayerCamera.ViewMatrix);
				spdlog::info("Mouse pressed at screen: ({}, {}), world: ({}, {})!", 
					mousePosition.x, mousePosition.y, worldPosition.x, worldPosition.y);
				
				// Try to click the login button
				if (loginButton.second->TryClick(worldPosition)) {
					// remove focus from all input boxes
					for (auto& box : inputBoxes) {
						box.second->SetFocused(false);
						focusedBox = INT_MAX;
					}
					return;
				}

				// Try to click any of the input boxes
				// loop through input boxes, i need the value and the index
				for (int i = 0; i < inputBoxes.size(); i++) {
					auto& box = inputBoxes[i];
					if (box.second->TryClick(worldPosition)) {
						if (!box.second->GetFocused()) {
							if (focusedBox != INT_MAX) {
								inputBoxes[focusedBox].second->SetFocused(false);
							}
							focusedBox = i;
							box.second->SetFocused(true);
						}
						return;
					}
				}

				// unfocus all input boxes
				for (auto& box : inputBoxes) {
					box.second->SetFocused(false);
					focusedBox = INT_MAX;
				}
			},
			.OnReleased = [this]() {
				spdlog::info("Mouse released!");
			}
		}
	});
}

void MMOTitleScreen::Init() {
	SceneLayer::Init();
	LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), // Camera position
	                                     glm::vec3(0.f, 0.f, 0.f), // Target to look at
	                                     glm::vec3(0.f, 1.f, 0.f)); // Up vector

	OZZ::game::objects::TextInput::TextInputParams textInputParams{};
	textInputParams.FontPath = "assets/fonts/PrintBold.ttf";
	textInputParams.FontSize = 36;
	textInputParams.Size = {600.f, 50.f};
	textInputParams.BackgroundColor = {1.0f, 0.2f, 0.2f, 0.5f};
	textInputParams.TextAnchorPoint = AnchorPoint::LeftMiddle;
	// bright blue border color
	textInputParams.FocusedColor = {1.f, 0.2f, 1.f, 1.f};
	textInputParams.FocusedThickness = glm::vec4{5.f};

	inputBoxes.emplace_back(gameWorld->CreateGameObject<OZZ::game::objects::TextInput>(textInputParams));
	inputBoxes.back().second->SetupInput(input.get());
	inputBoxes.back().second->SetFocused(true);
	inputBoxes.back().second->SetPosition({-0.f, -75.f, 0.f});

	textInputParams.bIsPassword = true;
	textInputParams.FontSize = 24;
	textInputParams.TextAnchorPoint = AnchorPoint::LeftMiddle;
	inputBoxes.emplace_back(gameWorld->CreateGameObject<OZZ::game::objects::TextInput>(textInputParams));
	inputBoxes.back().second->SetupInput(input.get());
	inputBoxes.back().second->SetFocused(false);
	inputBoxes.back().second->SetPosition({-0.f, -175.f, 0.f});
	focusedBox = 0;

	titleLabel = gameWorld->CreateGameObject<OZZ::game::objects::TextLabel>(
		fontPath, 128, "Lights", glm::vec3(1.f, 0.f, 0.f));
	titleLabel.second->SetPosition({0.f, 100, 0.f});
	titleLabel.second->SetScale(glm::vec3{1.5f, 1.5f, 1.f});

	usernameLabel = gameWorld->CreateGameObject<OZZ::game::objects::TextLabel>(
		fontPath, 32, "Username", glm::vec3(1.f, 0.f, 0.f));
	usernameLabel.second->SetPosition({-220.f, -25.f, 0.f});
	passwordLabel = gameWorld->CreateGameObject<OZZ::game::objects::TextLabel>(
		fontPath, 32, "Password", glm::vec3(1.f, 0.f, 0.f));
	passwordLabel.second->SetPosition({-220.f, -125.f, 0.f});

	// login button
	OZZ::game::objects::Button::ButtonParams buttonParams = {};
	buttonParams.Text = "Log In";
	buttonParams.FontPath = "assets/fonts/PrintBold.ttf";
	buttonParams.FontSize = 36;
	buttonParams.Size = {600.f, 50.f};
	buttonParams.BackgroundColor = {0.2f, 0.2f, 0.2f, 0.5f};
	buttonParams.OnClick = [this]() {
		spdlog::info("Login button clicked!");
	};

	loginButton = gameWorld->CreateGameObject<OZZ::game::objects::Button>(buttonParams);
	loginButton.second->SetPosition({-0.f, -325.f, 0.f});
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

	static glm::vec3 titleScreenOffset{0.f, 0.f, 0.f};
	static float totalTime = 1.f;

	totalTime += DeltaTime;
	// apply a sin wave
	titleScreenOffset.y = std::sin(totalTime * 2.f) * 32 + height / 4;

	titleLabel.second->SetPosition(titleScreenOffset);
}


void MMOTitleScreen::RenderTargetResized(glm::ivec2 size) {
	width = size.x;
	height = size.y;

	LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.001f,
	                                          1000.f);
}

std::vector<OZZ::scene::SceneObject> MMOTitleScreen::GetSceneObjects() {
	auto titleScreenObjects = std::vector<OZZ::scene::SceneObject>();
	for (const auto val: inputBoxes | std::views::values) {
		titleScreenObjects += val->GetSceneObjects();
	}
	titleScreenObjects += titleLabel.second->GetSceneObjects()
		+ usernameLabel.second->GetSceneObjects()
		+ passwordLabel.second->GetSceneObjects()
		+ loginButton.second->GetSceneObjects();
	return titleScreenObjects;
}

void MMOTitleScreen::selectNextInputBox(int direction) {
	// delesect the current box
	if (focusedBox < inputBoxes.size()) {
		inputBoxes[focusedBox].second->SetFocused(false);
	}
	focusedBox += direction;

	// if we're at the end of the list plus one, no selection
	if (direction > 0 && focusedBox > inputBoxes.size() - 1) {
		focusedBox = 0;
	} else {
		if (focusedBox < 0) {
			focusedBox = inputBoxes.size() - 1;
		}
	}

	spdlog::info("Selected box: {}", focusedBox);
	if (focusedBox < inputBoxes.size()) {
		inputBoxes[focusedBox].second->SetFocused(true);
	}
}
