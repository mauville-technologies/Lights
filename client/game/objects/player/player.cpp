//
// Created by ozzadar on 2025-03-24.
//

#include "player.h"

#include <utility>
#include <game/abilities/ability_system_component.h>
#include <game/abilities/ability/jump_ability.h>
#include <lights/game/game_world.h>
#include <lights/game/2D/sprite.h>
#include <lights/scene/constants.h>

namespace OZZ::game::objects {
	Player::Player(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld) : GameObject(
		inGameWorld, std::move(inPhysicsWorld)) {
		// let's create the pepe sprite
		sprite = inGameWorld->CreateGameObject<scene::Sprite>("assets/textures/pepe.png");
		sprite.second->AddBody(
			BodyType::Dynamic,
			OzzRectangle{
				.Position = glm::vec3{0, 2.f * constants::PixelsPerMeter, 0.f},
				.Size = glm::vec3{2.f * constants::PixelsPerMeter, 2.f * constants::PixelsPerMeter, 1.f}
			},
			glm::vec2{0, 0});

		abilitySystem = std::make_unique<abilities::AbilitySystemComponent>();
		abilitySystem->GrantAbility<abilities::JumpAbility>("Jump", sprite.second,
		                                                    abilities::JumpParameters {
			                                                    .JumpStrength = 20.f
		                                                    },
		                                                    [] {
			                                                    // TODO: Implement a proper isOnGround function
			                                                    return true;
		                                                    });

		abilitySystem->OnAbilityActivated = [this](const std::string &abilityName) {
			if (abilityName == "Jump") {
				spdlog::info("Jump ability activated");
			}
		};
	}

	Player::~Player() {
		if (gameWorld) {
			gameWorld->RemoveObject(sprite.first);
		}
	}

	void Player::Tick(float DeltaTime) {
		abilitySystem->Tick(DeltaTime);
		Position = sprite.second->GetPosition();

		const auto moveValue = inputSubsystem->GetAxisValue("MoveLeftRight");
		MoveLeft(moveValue);
	}

	std::vector<OZZ::scene::SceneObject> Player::GetSceneObjects() {
		return sprite.second->GetSceneObjects();
	}

	void Player::SetupInput(InputSubsystem *input) {
		inputSubsystem = input;
		input->RegisterInputMapping({
			.Action = "Jump",
			.Chords = {
				InputChord{.Keys = std::vector<InputKey>{{0, EControllerButton::A}}},
				InputChord{.Keys = std::vector<InputKey>{{-1, EKey::Space}}},
			},
			.Callbacks = {
				.OnPressed = [this]() {
					abilitySystem->ActivateAbility("Jump");
				},
				.OnReleased = [this]() {
				}
			}
		});

		input->RegisterAxisMapping({
			.Action = "MoveLeftRight",
			.Keys = {
				{{-1, EKey::Left}, -1.f},
				{{-1, EKey::Right}, 1.f},
				{{-1, EKey::A}, -1.f},
				{{-1, EKey::D}, 1.f},
				{{0, EControllerButton::LeftStickX}, 1.f},
			},
		});
	}

	void Player::Jump() const {
		if (auto *body = sprite.second->GetBody()) {
			body->Velocity.y = 20;
		}
	}

	void Player::MoveLeft(float value) const {
		if (auto *body = sprite.second->GetBody()) {
			body->Velocity.x = value * 10;
		}
	}
} // OZZ
