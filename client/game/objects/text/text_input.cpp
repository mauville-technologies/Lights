//
// Created by ozzadar on 2025-04-13.
//

#include "text_input.h"

#include <lights/game/game_world.h>
#include <spdlog/spdlog.h>

namespace OZZ::game::objects {
	TextInput::TextInput(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld, const TextInputParams inParams)
		: GameObject(inGameWorld, std::move(inPhysicsWorld)), params(inParams) {

		// create a text label
		label = gameWorld->CreateGameObject<TextLabel>(
			std::filesystem::path("assets/fonts/game_bubble.ttf"), 32, "", glm::vec3{1.f, 1.f, 1.f}, AnchorPoint::CenterLeft);
	}

	TextInput::~TextInput() {
		if (inputSubsystem) {
			inputSubsystem->UnregisterTextListener(inputSID);
			inputSubsystem->UnregisterInputMapping(inputSID);
			spdlog::info("TextInput::UnregisterTextListener. SID: {}", inputSID);
		}
		spdlog::info("TextInput destroyed");
	}

	void TextInput::Tick(float DeltaTime) {
	}

	std::vector<OZZ::scene::SceneObject> TextInput::GetSceneObjects() {
		return {label.second->GetSceneObjects()};
	}

	void TextInput::SetupInput(InputSubsystem *inInputSubsystem) {
		inputSubsystem = inInputSubsystem;

		spdlog::info("TextInput::SetupInput. isNull {}", inInputSubsystem == nullptr);
		const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		std::random_device rd; // Seed
		std::mt19937 generator(rd()); // Random number generator
		std::uniform_int_distribution<size_t> distribution(0, characters.size() - 1);

		inputSID = "";
		for (size_t i = 0; i < 24; ++i) {
			inputSID += characters[distribution(generator)];
		}

		// listen to text callbacks
		inputSubsystem->RegisterTextListener({
			.Name = inputSID,
			.Callback = [this] (const char character) {
				appendCharacter(character);
			},
		});

		// register backspace
		inputSubsystem->RegisterInputMapping({
			.Action = inputSID,
			.Chords = {
				InputChord{
					.Keys = {{-1, EKey::BackSpace}},
					.bIsSequence = false,
					.bCanRepeat = true
				}
			},
			.Callbacks = {
				.OnPressed = [this]() {
					removeCharacter();
				}
			}
		});
	}

	void TextInput::appendCharacter(const char character) {
		currentString += character;
		label.second->SetText(currentString);
	}

	void TextInput::removeCharacter() {
		if (!currentString.empty()) {
			currentString.pop_back();
			label.second->SetText(currentString);
		}
	}
}