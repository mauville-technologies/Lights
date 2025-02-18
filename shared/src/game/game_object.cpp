//
// Created by ozzadar on 2024-12-31.
//

#include "lights/game/game_object.h"

namespace OZZ {
    GameObject::GameObject(std::shared_ptr<OzzWorld2D> InWorld) : Position(0.f), Scale(1.f), Rotation(),
                                                                           world(std::move(InWorld)) {
    }
} // OZZ
