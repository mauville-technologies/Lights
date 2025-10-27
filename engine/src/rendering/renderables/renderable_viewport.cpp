//
// Created by paulm on 2025-10-21.
//

#include "renderable_viewport.h"

namespace OZZ {
    RenderableViewport::RenderableViewport() {
    }

    void RenderableViewport::Init() {
        // create the render target
        auto newTarget = OZZ::RenderTarget(OZZ::RenderTargetType::Viewport);
        renders.insert(std::make_pair(GetName(), newTarget));

        // quad to render to
    }

    bool RenderableViewport::render() {
        return false;
    }
} // OZZ
