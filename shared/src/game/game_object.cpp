//
// Created by ozzadar on 2024-12-31.
//

#include "lights/game/game_object.h"

namespace OZZ {
	GameObject::GameObject(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld) : position(0.f), scale(1.f),
		rotation(),
		gameWorld(inGameWorld),
		physicsWorld(std::move(inPhysicsWorld)) {
		updateTransform();
	}

	glm::mat4 GameObject::GetWorldTransform() const { return parent ? parent->GetWorldTransform() * transform : transform; }

	void GameObject::updateTransform() {
		transform = glm::translate(glm::mat4{1.f}, position);
		transform = glm::scale(transform, scale);
		transform = glm::rotate(transform, rotation.x, glm::vec3(1, 0, 0));
	}
} // OZZ
