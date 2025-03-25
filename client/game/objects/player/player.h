//
// Created by ozzadar on 2025-03-24.
//

#pragma once
#include <lights/game/game_object.h>

namespace OZZ::game::abilities {
	class AbilitySystemComponent;
}

namespace OZZ {
	class InputSubsystem;
}

// TODO: Leaving a note here that game::scene is conflicting with OZZ::scene, which is not fun to deal with
namespace OZZ::game::scene {
	class Sprite;
}

namespace OZZ::game::objects {
	class Player final : public GameObject{
	public:
		explicit Player(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld);
		~Player() override;
		void Tick(float DeltaTime) override;
		std::vector<OZZ::scene::SceneObject> GetSceneObjects() override;

		void SetupInput(InputSubsystem* input);

	private:
		void Jump() const;
		void MoveLeft(float value) const;

	private:
		InputSubsystem *inputSubsystem { nullptr };
		std::unique_ptr<abilities::AbilitySystemComponent> abilitySystem { nullptr };
		std::pair<uint64_t, scene::Sprite*> sprite {UINT64_MAX, nullptr};
	};
} // OZZ
