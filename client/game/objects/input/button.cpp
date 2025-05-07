//
// Created by ozzadar on 2025-04-13.
//

#include "button.h"

#include <lights/game/game_world.h>
#include <lights/scene/scene.h>
#include <spdlog/spdlog.h>

namespace OZZ::game::objects {
	Button::Button(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld, const ButtonParams &inParams)
		: GameObject(inGameWorld, std::move(inPhysicsWorld)), params(inParams) {

		// create a text label
		label = gameWorld->CreateGameObject<TextLabel>(
			params.FontPath, params.FontSize, currentString, params.TextColor, AnchorPoint::CenterMiddle);
		label.second->SetParent(this);
		auto textPosition = glm::vec3{0.f};
		// textPosition.x = -params.Size.x / 2;
		// textPosition.y = params.Size.y / 2;
		currentString = params.Text;
		setText(currentString);
		label.second->SetPosition(textPosition);
		label.second->SetRectBounds({params.Size.x, params.Size.y});

		// let's build a quad of the correct size
		const auto backgroundMesh = std::make_shared<IndexVertexBuffer>();
		auto vertices = std::vector<Vertex>();
		auto indices = std::vector<uint32_t>();

		vertices.push_back(Vertex {
			.position = {
				-params.Size.x /2, -params.Size.y /2, 0.f
			}
		});
		vertices.push_back(Vertex {
			.position = {
				params.Size.x /2, -params.Size.y /2, 0.f
			}
		});
		vertices.push_back(Vertex {
			.position = {
				-params.Size.x /2, params.Size.y /2, 0.f
			}
		});
		vertices.push_back(Vertex {
			.position = {
				params.Size.x /2, params.Size.y /2, 0.f
			}
		});

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		indices.push_back(2);
		backgroundMesh->UploadData(vertices, indices);

		auto backgroundShader = std::make_shared<Shader>("assets/shaders/text/input_rectangle.vert", "assets/shaders/text/input_rectangle.frag");
		auto backgroundMaterial = std::make_shared<Material>();
		backgroundMaterial->SetShader(backgroundShader);
		backgroundMaterial->AddUniformSetting({
			.Name = "backgroundColor",
			.Value = params.BackgroundColor,
		});

		backgroundMaterial->AddUniformSetting({
			.Name = "rectBounds",
			.Value = glm::vec4{-params.Size.x/2, -params.Size.y/2, params.Size.x/2, params.Size.y/2},
		});

		backgroundBox = scene::SceneObject {
			.Transform = glm::mat4{1.f},
			.Mesh = backgroundMesh,
			.Mat = backgroundMaterial,
		};

		Button::onPositionChanged();
	}

	Button::~Button() {
		if (inputSubsystem) {
			inputSubsystem->UnregisterTextListener(inputSID);
			inputSubsystem->UnregisterInputMapping(inputSID);
			spdlog::info("Button::UnregisterTextListener. SID: {}", inputSID);
		}
		spdlog::info("Button destroyed");
	}

	void Button::Tick(float DeltaTime) {
		onPositionChanged();
	}

	std::vector<OZZ::scene::SceneObject> Button::GetSceneObjects() {
		auto labelObjects = label.second->GetSceneObjects();
		auto backgroundObjects = std::vector<OZZ::scene::SceneObject>{backgroundBox};
		return backgroundObjects + labelObjects;
	}

	void Button::SetupInput(InputSubsystem *inInputSubsystem) {
		inputSubsystem = inInputSubsystem;

		spdlog::info("Button::SetupInput. isNull {}", inInputSubsystem == nullptr);
		const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		std::random_device rd; // Seed
		std::mt19937 generator(rd()); // Random number generator
		std::uniform_int_distribution<size_t> distribution(0, characters.size() - 1);

		inputSID = "";
		for (size_t i = 0; i < 24; ++i) {
			inputSID += characters[distribution(generator)];
		}
	}

	void Button::updateTextLabel() const {
		label.second->SetText(currentString);
	}

	void Button::setText(const std::string &inText) {
		currentString = inText;
		updateTextLabel();
	}

	void Button::SetFocused(const bool focused) {
		isFocused = focused;

		if (const auto backgroundMaterial = backgroundBox.Mat) {
			backgroundMaterial->AddUniformSetting({
				.Name = "borderColor",
				.Value = params.FocusedColor,
			});
			backgroundMaterial->AddUniformSetting({
				.Name = "borderThickness",
				.Value = focused ? params.FocusedThickness : glm::vec4{0},
			});
		}
	}

	void Button::onPositionChanged() {
		GameObject::onPositionChanged();

		backgroundBox.Transform = GetWorldTransform();
	}
}
