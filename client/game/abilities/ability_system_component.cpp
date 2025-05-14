//
// Created by ozzadar on 2025-03-24.
//

#include "ability_system_component.h"

#include <ranges>

namespace OZZ::game::abilities {
	void AbilitySystemComponent::RevokeAbility(const std::string &AbilityName) {
		if (Abilities.contains(AbilityName)) {
			Abilities.erase(AbilityName);
			if (OnAbilityRevoked) {
				OnAbilityRevoked(AbilityName);
			}
		}
	}

	void AbilitySystemComponent::ActivateAbility(const std::string &AbilityName) {
		if (Abilities.contains(AbilityName)) {
			if (OnAbilityActivated && Abilities[AbilityName]->Activate()) {
				Abilities[AbilityName]->Execute();
				OnAbilityActivated(AbilityName);
			}
		}
	}

	void AbilitySystemComponent::Tick(float DeltaTime) {
		// Tick all abilities
		for (const auto &ability: Abilities | std::views::values) {
			ability->Tick(DeltaTime);
		}
	}
}
