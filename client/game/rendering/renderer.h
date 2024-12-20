//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <memory>
#include "game/scene/scene.h"
#include "buffer.h"
#include "shader.h"
#include "lights/util/image.h"
#include "texture.h"
#include "material.h"

namespace OZZ {
    class Renderer {
    public:
        Renderer();

        void RenderScene(Scene* scene);
    };
}