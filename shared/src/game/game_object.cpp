//
// Created by ozzadar on 2024-12-31.
//

#include "lights/game/game_object.h"

namespace OZZ {
    GameObject::GameObject(std::shared_ptr<CollisionSystem> InCollision) : position(0.f), scale(1.f), rotation(),
                                                                           collisionSystem(std::move(InCollision)) {
    }
} // OZZ
