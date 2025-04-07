//
// Created by ozzadar on 2025-04-06.
//

#include "text_label.h"

namespace OZZ::game::objects {
	TextLabel::TextLabel(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld)
	: GameObject(inGameWorld, std::move(inPhysicsWorld)) {

	}
}
