//
// Created by Paul Mauviel on 2025-02-20.
//

#pragma once

#include <ozz_collision/shapes/ozz_circle.h>
#include <ozz_collision/shapes/ozz_line.h>
#include <ozz_collision/shapes/ozz_point.h>
#include <ozz_collision/shapes/ozz_rectangle.h>
#include <variant>

namespace OZZ::collision::shapes {
    // The order of the enums here need to match the index order of the variant in OzzShapeData
    // this is to do a quick lookup of the shape type using .index()
    enum class OzzShapeKind { Unknown = -1, Point, Rectangle, Circle, Line };

    using OzzShapeData = std::variant<OzzPoint, OzzRectangle, OzzCircle, OzzLine>;

} // namespace OZZ::collision::shapes
