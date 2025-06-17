//
// Created by ozzadar on 2025-04-13.
//

#include "text_input.h"

#include <lights/game/game_world.h>
#include <lights/scene/scene.h>
#include <spdlog/spdlog.h>

namespace OZZ::game::objects {
	TextInput::TextInput(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld,
	                     const TextInputParams &inParams)
		: UIComponent(inGameWorld, std::move(inPhysicsWorld)), params(inParams) {
		currentString = "";
		// create a text label
		label = gameWorld->CreateGameObject<TextLabel>(TextLabel::ParamsType{
				.FontPath = params.FontPath,
				.FontSize = params.FontSize,
				.Text = currentString,
				.Color = params.TextColor,
				.AnchorPoint = params.TextAnchorPoint
			}
		);

		label.second->SetParent(this);
		label.second->SetRectBounds({params.Size.x, params.Size.y});

		// let's build a quad of the correct size
		auto backgroundMesh = std::make_shared<IndexVertexBuffer>();
		auto vertices = std::vector<Vertex>();
		auto indices = std::vector<uint32_t>();

		vertices.push_back(Vertex{
			.position = {
				-params.Size.x / 2, -params.Size.y / 2, 0.f
			}
		});
		vertices.push_back(Vertex{
			.position = {
				params.Size.x / 2, -params.Size.y / 2, 0.f
			}
		});
		vertices.push_back(Vertex{
			.position = {
				-params.Size.x / 2, params.Size.y / 2, 0.f
			}
		});
		vertices.push_back(Vertex{
			.position = {
				params.Size.x / 2, params.Size.y / 2, 0.f
			}
		});

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);
		backgroundMesh->UploadData(vertices, indices);

		auto backgroundShader = std::make_shared<Shader>("assets/shaders/text/input_rectangle.vert",
		                                                 "assets/shaders/text/input_rectangle.frag");
		auto backgroundMaterial = std::make_shared<Material>();
		backgroundMaterial->SetShader(backgroundShader);
		backgroundMaterial->AddUniformSetting({
			.Name = "backgroundColor",
			.Value = params.BackgroundColor,
		});

		backgroundMaterial->AddUniformSetting({
			.Name = "rectBounds",
			.Value = glm::vec4{-params.Size.x / 2, -params.Size.y / 2, params.Size.x / 2, params.Size.y / 2},
		});

		backgroundBox = scene::SceneObject{
			.Transform = glm::mat4{1.f},
			.Mesh = backgroundMesh,
			.Mat = backgroundMaterial,
		};
		TextInput::onPositionChanged();
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
		auto labelObjects = label.second->GetSceneObjects();
		auto backgroundObjects = std::vector<OZZ::scene::SceneObject>{backgroundBox};
		return backgroundObjects + labelObjects;
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
			.Callback = [this](const char character) {
				if (!IsFocused()) return;
				appendCharacter(character);
			},
		});

		// register backspace
		inputSubsystem->RegisterInputMapping({
			.Action = inputSID,
			.Chords = {
				InputChord{
					.Keys = {{EDeviceID::Keyboard, EKey::BackSpace}},
					.bIsSequence = false,
					.bCanRepeat = true
				}
			},
			.Callbacks = {
				.OnPressed = [this]() {
					if (!IsFocused()) return;
					removeCharacter();
				}
			}
		});
	}

	void TextInput::updateTextLabel() const {
		const auto text = params.bIsPassword ? std::string(currentString.size(), '*') : currentString;
		label.second->SetText(text);
	}

	void TextInput::appendCharacter(const char character) {
		currentString += character;
		updateTextLabel();
	}

	void TextInput::removeCharacter() {
		if (!currentString.empty()) {
			currentString.pop_back();
			updateTextLabel();
		}
	}

	bool TextInput::IsMouseOver(const glm::vec2 &worldPos) {
		const auto &inputPos = GetWorldPosition();
		const auto &inputSize = params.Size;

		if (worldPos.x >= inputPos.x - inputSize.x / 2 &&
		    worldPos.x <= inputPos.x + inputSize.x / 2 &&
		    worldPos.y >= inputPos.y - inputSize.y / 2 &&
		    worldPos.y <= inputPos.y + inputSize.y / 2) {
			;
			// TODO: If we are focused, we should try and move the cursor to the clicked position
			return true;
		}
		return false;
	}

	void TextInput::Clicked() {
	}

	void TextInput::onFocusChanged() {
		if (auto backgroundMaterial = backgroundBox.Mat) {
			backgroundMaterial->AddUniformSetting({
				.Name = "borderColor",
				.Value = params.FocusedColor,
			});
			backgroundMaterial->AddUniformSetting({
				.Name = "borderThickness",
				.Value = IsFocused() ? params.FocusedThickness : glm::vec4{0},
			});
		}
	}

	void TextInput::onPositionChanged() {
		GameObject::onPositionChanged();

		// update the transform of the background box
		backgroundBox.Transform = GetWorldTransform();

		if (label.second) {
			auto textPosition = glm::vec3{0.f};
			switch (params.TextAnchorPoint) {
				case AnchorPoint::LeftTop:
				case AnchorPoint::LeftBottom:
				case AnchorPoint::LeftMiddle:
					textPosition.x = -params.Size.x / 2;
					break;
				case AnchorPoint::RightTop:
				case AnchorPoint::RightBottom:
				case AnchorPoint::RightMiddle:
					textPosition.x = params.Size.x / 2;
					break;
				case AnchorPoint::CenterMiddle:
				case AnchorPoint::CenterTop:
				case AnchorPoint::CenterBottom:
					break;
			}

			switch (params.TextAnchorPoint) {
				case AnchorPoint::LeftTop:
				case AnchorPoint::RightTop:
				case AnchorPoint::CenterTop:
					textPosition.y = params.Size.y / 2;
					break;
				case AnchorPoint::LeftBottom:
				case AnchorPoint::RightBottom:
				case AnchorPoint::CenterBottom:
					textPosition.y = -params.Size.y / 2;
					break;
				case AnchorPoint::LeftMiddle:
				case AnchorPoint::RightMiddle:
				case AnchorPoint::CenterMiddle:
					break;
			}

			label.second->SetPosition(textPosition);
		}
	}
}
