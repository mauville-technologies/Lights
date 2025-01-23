//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <memory>
#include "lights/rendering/buffer.h"
#include "lights/rendering/shader.h"
#include "lights/rendering/texture.h"
#include "lights/rendering/material.h"

#include "lights/scene/scene.h"

#include "lights/util/image.h"

namespace OZZ {
    class Renderer {
    public:
        Renderer();

        void RenderScene(Scene* scene);
    };
}