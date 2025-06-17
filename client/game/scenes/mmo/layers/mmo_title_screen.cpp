//
// Created by ozzadar on 2025-04-06.
//

#include "mmo_title_screen.h"

#include <ranges>
#include <game/library/layers/ui_layer.h>
#include <game/library/objects/ui/input/button.h>
#include <game/library/objects/ui/input/text_input.h>
#include <game/library/objects/ui/text/text_label.h>
#include <lights/rendering/shapes.h>
#include <game/utils/mouse_utils.h>

MMOTitleScreen::MMOTitleScreen(OZZ::GameWorld *inWorld, OZZ::lights::library::layers::UILayer* inUILayer) : gameWorld(inWorld), uiLayer(inUILayer) {}

void MMOTitleScreen::SetInputSubsystem(const std::shared_ptr<OZZ::InputSubsystem> &inInput) {
	input = inInput;
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

	const auto UsernameBox = uiLayer->AddComponent<OZZ::game::objects::TextInput>("UsernameBox", textInputParams, true);
	UsernameBox.second->SetupInput(input.get());
	UsernameBox.second->SetFocused(true);
	UsernameBox.second->SetPosition({-0.f, -75.f, 0.f});

	textInputParams.bIsPassword = true;
	textInputParams.FontSize = 24;
	textInputParams.TextAnchorPoint = AnchorPoint::LeftMiddle;

	const auto PasswordBox = uiLayer->AddComponent<OZZ::game::objects::TextInput>("PasswordBox", textInputParams, true);
	PasswordBox.second->SetupInput(input.get());
	PasswordBox.second->SetFocused(false);
	PasswordBox.second->SetPosition({-0.f, -175.f, 0.f});

	titleLabel = uiLayer->AddComponent<OZZ::game::objects::TextLabel>("TitleLabel",
        OZZ::game::objects::TextLabel::ParamsType{
            .FontPath = fontPath,
            .FontSize = 128,
            .Text = "Lights",
            .Color = {1.f, 0.f, 0.f}
        });

	titleLabel.second->SetPosition({0.f, 100, 0.f});
	titleLabel.second->SetScale(glm::vec3{1.5f, 1.5f, 1.f});

	const auto UsernameLabel = uiLayer->AddComponent<OZZ::game::objects::TextLabel>("UsernameLabel",
        OZZ::game::objects::TextLabel::ParamsType{
            .FontPath = fontPath,
            .FontSize = 32,
            .Text = "Username",
            .Color = {1.f, 0.f, 0.f}
        });
	UsernameLabel.second->SetPosition({-220.f, -25.f, 0.f});

	const auto PasswordLabel = uiLayer->AddComponent<OZZ::game::objects::TextLabel>("PasswordLabel",
        OZZ::game::objects::TextLabel::ParamsType{
            .FontPath = fontPath,
            .FontSize = 32,
            .Text = "Password",
            .Color = {1.f, 0.f, 0.f}
        });
	PasswordLabel.second->SetPosition({-220.f, -125.f, 0.f});

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

	const auto LoginButton = uiLayer->AddComponent<OZZ::game::objects::Button>("LoginButton", buttonParams, true);
	LoginButton.second->SetPosition({-0.f, -325.f, 0.f});
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
	return {};
}
