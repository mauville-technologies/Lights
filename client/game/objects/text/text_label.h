//
// Created by ozzadar on 2025-04-06.
//

#pragma once
#include <lights/game/game_object.h>

namespace OZZ::game::objects {
	class TextLabel : public GameObject {
		explicit TextLabel(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld);
	};
} // game
