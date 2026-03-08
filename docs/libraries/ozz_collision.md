# Library: ozz_collision

Path: `libs/ozz_collision/*`

## Purpose

`ozz_collision` provides a 2D collision domain:
- primitive shapes;
- shape-vs-shape collision checks;
- simple world/body simulation + query support.

## Core Types

- shapes:
  - `OzzPoint`
  - `OzzRectangle`
  - `OzzCircle`
  - `OzzLine`
- `OzzShapeData` variant + `OzzShapeKind`
- `OzzCollisionResult`:
  - `bCollided`
  - `ContactPoints`
  - `CollisionNormal`
  - `PenetrationDepth`

## World Model (`OzzWorld2D`)

World responsibilities:
- create/destroy/get bodies;
- step simple physics each tick;
- detect and resolve collisions between dynamic/static/kinematic bodies;
- run point queries.

## Runtime Characteristics (concrete)

- dynamic bodies apply gravity and velocity integration;
- collision checks delegate to shape overloads via variant dispatch;
- simple response resolves penetration and cancels velocity components against collision normal;
- body IDs are randomized and uniqueness-checked.

## Inferred Design Intent

- provide a practical lightweight collision/physics substrate for gameplay systems;
- keep shape collision implementations explicit and testable;
- expose enough collision detail for gameplay response logic.

## Speculative Direction (labeled)

Likely expansion points:
- broader collision pair completeness;
- richer resolution and material/response parameters;
- spatial acceleration structures for larger world scales.
