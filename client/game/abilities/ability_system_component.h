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
	using OnAbilityGrantedFunction = std::function<void(const std::string& AbilityName)>;
	using OnAbilityRevokedFunction = std::function<void(const std::string& AbilityName)>;
	using OnAbilityActivatedFunction = std::function<void(const std::string& AbilityName)>;
	using OnAbilityActivationFailedFunction = std::function<void(const std::string& AbilityName, const std::string& Reason)>;

	class AbilitySystemComponent {
	public:
		template <typename T, typename... Args>
		T* GrantAbility(const std::string& AbilityName, Args&&... args) {
			auto ability = std::make_unique<T>(std::forward<Args>(args)...);
			Abilities[AbilityName] = std::move(ability);
			if (OnAbilityGranted) OnAbilityGranted(AbilityName);
			return dynamic_cast<T*>(Abilities[AbilityName].get());
		};

		template <typename T>
		T* GetAbility(const std::string& AbilityName) {
			if (Abilities.contains(AbilityName)) {
				return dynamic_cast<T*>(Abilities[AbilityName].get());
			}
			return nullptr;
		}

		void RevokeAbility(const std::string& AbilityName);
		void ActivateAbility(const std::string& AbilityName);

		void Tick(float DeltaTime);

	public:
		OnAbilityGrantedFunction OnAbilityGranted;
		OnAbilityRevokedFunction OnAbilityRevoked;
		OnAbilityActivatedFunction OnAbilityActivated;
		OnAbilityActivationFailedFunction OnAbilityActivationFailed;

	private:
		std::unordered_map<std::string, std::unique_ptr<Ability>> Abilities;
	};
} // abilities
