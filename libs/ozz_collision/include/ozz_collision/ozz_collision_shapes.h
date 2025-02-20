//
// Created by Paul Mauviel on 2025-02-20.
//

#pragma once

#include <ozz_collision/shapes/ozz_circle.h>
#include <ozz_collision/shapes/ozz_rectangle.h>
#include <ozz_collision/shapes/ozz_point.h>


namespace OZZ::collision::shapes {
    enum class OzzShapeKind {
        Unknown,
        Circle,
        Rectangle,
        Point
    };

    using OzzShapeData = std::variant<OzzPoint, OzzRectangle, OzzCircle>;

    static glm::vec3 GetOzzShapePosition(OzzShapeKind kind, OzzShapeData data) {
        switch (kind) {
        case OzzShapeKind::Circle: {
            auto& typedData = std::get<OzzCircle>(data);
            return {typedData.Position, 1.f};
        }
        case OzzShapeKind::Rectangle: {
            auto& typedData = std::get<OzzRectangle>(data);
            return {typedData.Position, 1.f};
        }
        case OzzShapeKind::Point: {
            auto& typedData = std::get<OzzPoint>(data);
            return {typedData.Position, 1.f};
        }
        default:

        }
        return {};
    }

    static void SetOzzShapePosition(OzzShapeKind kind, OzzShapeData& data, glm::vec3 position) {
        switch (kind) {
        case OzzShapeKind::Circle: {
            auto& typedData = std::get<OzzCircle>(data);
            typedData.Position = position;
            return;
        }
        case OzzShapeKind::Rectangle: {
            auto& typedData = std::get<OzzRectangle>(data);
            typedData.Position = position;
            return;
        }
        case OzzShapeKind::Point: {
            auto& typedData = std::get<OzzPoint>(data);
            typedData.Position = position;
            return;
        }
        default:

        }
    }
}
