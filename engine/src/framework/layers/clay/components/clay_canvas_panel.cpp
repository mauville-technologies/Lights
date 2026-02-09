//
// Created by ozzadar on 2025-09-20.
//

#include "lights/framework/layers/clay/components/clay_canvas_panel.h"

ClayCanvasPanel::ClayCanvasPanel(const ClayCanvasPanelSettings& inSettings) : settings(inSettings) {}

void ClayCanvasPanel::Render() {
    const ClayComponentLayout layout{
        .id = CLAY_ID("CanvasPanel"),
        .layout =
            {
                .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                .padding = Clay_Padding(settings.Padding.x, settings.Padding.y, settings.Padding.z, settings.Padding.w),
                .childGap = settings.ChildGap,
            },
        .backgroundColor = Clay_Color(settings.BackgroundColor.x,
                                      settings.BackgroundColor.y,
                                      settings.BackgroundColor.z,
                                      settings.BackgroundColor.w)};

    CLAY(layout) {
        ClayComponent::Render();
    }
}