//
// Created by ozzadar on 2025-03-24.
//

#include "jump_ability.h"

namespace OZZ::game::abilities {
	JumpAbility::JumpAbility(IsOnGroundFunction inIsOnGround) : isOnGround(std::move(inIsOnGround)) {}

	bool JumpAbility::Activate() {
		if (!OnCooldown() && isOnGround()) {
			return true;
		}
		return false;
	}

	void JumpAbility::Tick(float DeltaTime) {
	}

	bool JumpAbility::OnCooldown() {
		return false;
	}
}
