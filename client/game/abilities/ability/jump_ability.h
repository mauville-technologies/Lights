//
// Created by ozzadar on 2025-03-24.
//

#pragma once

#include "game/abilities/ability.h"
#include <functional>

namespace OZZ {
	namespace game::scene {
		class Sprite;
	}

	struct Body;
}

namespace OZZ::game::abilities {

	using IsOnGroundFunction = std::function<bool()>;

	struct JumpParameters {
		float JumpStrength;
	};

	class JumpAbility : public Ability {
	public:
		explicit JumpAbility(scene::Sprite* inSprite, JumpParameters&& params, IsOnGroundFunction inIsOnGround);
		bool Activate() override;
		void Execute() override;;
		void Tick(float DeltaTime) override;

	    JumpParameters& GetParameters() { return jumpParameters; };
	private:
		bool OnCooldown();
	private:
		IsOnGroundFunction isOnGround;
		scene::Sprite* sprite;
		JumpParameters jumpParameters;
	};

} // OZZ
