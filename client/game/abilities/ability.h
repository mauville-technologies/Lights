//
// Created by ozzadar on 2025-03-24.
//

#pragma once

namespace OZZ::game::abilities {
	class Ability {
	public:
		virtual ~Ability() = default;
		virtual bool Activate() = 0;
		virtual void Tick(float DeltaTime) = 0;
	};
}