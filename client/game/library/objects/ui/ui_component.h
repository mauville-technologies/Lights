//
// Created by ozzadar on 2025-05-22.
//

#pragma once
#include "lights/game/game_world.h"

namespace OZZ::game::objects {
    class UIComponent : public OZZ::GameObject {
    public:
        UIComponent(GameWorld *inWorld, const std::shared_ptr<OzzWorld2D> &ozz_world_2d)
            : GameObject(inWorld, ozz_world_2d) {
        }
    };
}

template<typename T>
concept HasParamsType = requires { typename T::ParamsType; };

template<typename T>
concept HasUIConstructor = requires(OZZ::GameWorld *inGameWorld, std::shared_ptr<OZZ::OzzWorld2D> inPhysicsWorld,
                                    const typename T::ParamsType &inParams) {
    T{inGameWorld, inPhysicsWorld, inParams}; // Check if T can be constructed with these parameters
};

template <typename T>
concept IsUIComponentType = std::derived_from<T, OZZ::game::objects::UIComponent> && HasParamsType<T> && HasUIConstructor<T>;

template<typename... Types>
concept IsUIComponent = (IsUIComponentType<Types> && ...);