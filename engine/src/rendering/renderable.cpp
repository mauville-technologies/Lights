//
// Created by paulm on 2025-10-21.
//

#include "lights/rendering/renderable.h"

std::optional<OZZ::RenderTarget*> OZZ::Renderable::GetRender(const std::string& name) {
    if (bRenderedThisFrame && renders.contains(name)) {
        if (&renders[name]) {
            // found, and rendered
            return renders[name];
        }
    }
    // not found or rendered yet
    return std::nullopt;
}

bool OZZ::Renderable::Render() {
    if (!HasAllRequiredInputs()) {
        spdlog::warn("Cannot render node {} due to missing inputs", GetRenderableName());
        return false;
    }

    if (render()) {
        bRenderedThisFrame = true;
        return true;
    }
    return false;
}

void OZZ::Renderable::ResetFrameState() {
    bRenderedThisFrame = false;
    newFrame();
}

bool OZZ::Renderable::HasAllRequiredInputs() {
    for (const auto& requiredInput : GetRequiredInputs()) {
        bool found = false;
        for (const auto& inputNode : inputs) {
            auto* renderableNode = static_cast<Renderable*>(inputNode);
            if (renderableNode->GetRender(requiredInput)) {
                found = true;
                break;
            }
        }
        if (!found) {
            spdlog::warn("Missing required input {} for node {}", requiredInput, GetRenderableName());
            return false;
        }
    }
    return true;
}
