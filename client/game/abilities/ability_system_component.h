//
// Created by ozzadar on 2025-03-24.
//

#pragma once
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include "ability.h"

namespace OZZ::game::abilities {
	using OnAbilityGranted = std::function<void(const std::string& AbilityName)>;
	using OnAbilityRevoked = std::function<void(const std::string& AbilityName)>;
	using OnAbilityActivated = std::function<void(const std::string& AbilityName)>;
	using OnAbilityActivationFailed = std::function<void(const std::string& AbilityName, const std::string& Reason)>;

	class AbilitySystemComponent {
	public:
		template <typename T, typename... Args>
		T* GrantAbility(const std::string& AbilityName, Args&&... args) {
			auto ability = std::make_unique<T>(std::forward<Args>(args)...);
			Abilities[AbilityName] = std::move(ability);
			if (OnAbilityGranted) OnAbilityGranted(AbilityName);
			return dynamic_cast<T*>(Abilities[AbilityName].get());
		};

		void RevokeAbility(const std::string& AbilityName);

		void ActivateAbility(const std::string& AbilityName);
		void Tick(float DeltaTime);

	public:
		OnAbilityGranted OnAbilityGranted;
		OnAbilityRevoked OnAbilityRevoked;
		OnAbilityActivated OnAbilityActivated;
		OnAbilityActivationFailed OnAbilityActivationFailed;

	private:
		std::unordered_map<std::string, std::unique_ptr<Ability>> Abilities;
	};
} // abilities
