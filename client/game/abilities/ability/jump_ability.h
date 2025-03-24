//
// Created by ozzadar on 2025-03-24.
//

#pragma once

#include "game/abilities/ability.h"
#include <functional>

namespace OZZ::game::abilities {

	using IsOnGroundFunction = std::function<bool()>;
	class JumpAbility : public Ability {
	public:
		explicit JumpAbility(IsOnGroundFunction inIsOnGround);
		bool Activate() override;
		void Tick(float DeltaTime) override;

	private:
		bool OnCooldown();
	private:
		IsOnGroundFunction isOnGround;
	};

} // OZZ
