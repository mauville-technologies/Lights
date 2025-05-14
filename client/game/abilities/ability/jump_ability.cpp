//
// Created by ozzadar on 2025-03-24.
//

#include "jump_ability.h"

#include <lights/game/2D/sprite.h>
#include <ozz_collision/world.h>

namespace OZZ::game::abilities {
	JumpAbility::JumpAbility(scene::Sprite *inSprite, JumpParameters &&params,
	                         IsOnGroundFunction inIsOnGround) : isOnGround(std::move(inIsOnGround)), sprite(inSprite), jumpParameters(params) {
	}

	bool JumpAbility::Activate() {
		if (!OnCooldown() && isOnGround()) {
			return true;
		}
		return false;
	}

	void JumpAbility::Execute() {
		if (auto *body = this->sprite->GetBody(); body) {
			body->Velocity.y += jumpParameters.JumpStrength;
		}
	}

	void JumpAbility::Tick(float DeltaTime) {
	}

	bool JumpAbility::OnCooldown() {
		return false;
	}
}
