//
// Created by ozzadar on 2024-12-31.
//

#include "lights/game/game_object.h"

namespace OZZ {
	GameObject::GameObject(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld) : Position(0.f), Scale(1.f),
		Rotation(),
		gameWorld(inGameWorld),
		physicsWorld(std::move(inPhysicsWorld)) {
	}
} // OZZ
