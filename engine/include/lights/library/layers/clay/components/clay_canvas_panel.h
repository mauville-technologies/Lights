//
// Created by ozzadar on 2025-09-20.
//

#pragma once
#include "clay_component.h"
#include "glm/vec4.hpp"

struct ClayCanvasPanelSettings {
    uint16_t ChildGap = 16;                // Gap between children
    glm::ivec4 Padding{16, 16, 16, 16};    // Padding inside the panel (left, right, top, bottom)
    glm::vec4 BackgroundColor{0, 0, 0, 0}; // Background color (RGBA)
};

/**
 * Class that encompasses the full window. Generally used as a root component.
 */
class ClayCanvasPanel : public ClayComponent<ClayCanvasPanel> {
public:
    explicit ClayCanvasPanel(const ClayCanvasPanelSettings& inSettings = {});
    void Render() override;

private:
    ClayCanvasPanelSettings settings;
};
