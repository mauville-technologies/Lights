//
// Created by ozzadar on 2025-09-07.
//

#include <lights/framework/layers/clay/components/clay_panel.h>

ClayPanel::ClayPanel(const ClayComponentLayout& inLayout) : layout(inLayout) {}

void ClayPanel::Render() {
    // Don't render if the panel has no size -- this is a quick fix for now
    if (layout.layout.sizing.width.type == Clay__SizingType::CLAY__SIZING_TYPE_FIXED &&
        layout.layout.sizing.height.type == Clay__SizingType::CLAY__SIZING_TYPE_FIXED &&
        layout.layout.sizing.width.size.minMax.max == 0 && layout.layout.sizing.height.size.minMax.max == 0) {
        return;
    }
    CLAY(layout) {
        ClayComponent::Render();
    }
}