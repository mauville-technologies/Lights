//
// Created by ozzadar on 2025-09-07.
//

#pragma once
#include "clay_component.h"
#include "spdlog/spdlog.h"

class ClayPanel final : public ClayComponent<ClayPanel> {
public:
    explicit ClayPanel(const ClayComponentLayout& inLayout);

    void Render() override;

protected:
private:
    ClayComponentLayout layout;
};
