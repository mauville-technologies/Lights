# Closed Tasks

## 2025-01-11
Box2D was implemented with Tiled support up to this point. I decided to strip out Box2D and implement my own basic collision 
system that will liklye integrate better and be easier to maintain.

- [x] World
    - [x] Gameobjects and ECS?
- [x] Pick a 2D Physics Engine
    - [ ] Camera Follows Player
    - [x] Integrate the engine
    - [x] Create a player
    - [x] Move the character
    - [x] Player can jump
- [x] Use Tiled to create initial world.
    -  [x] Read Tiled-generated map (https://github.com/SSBMTonberry/tileson)
    -  [x] Build / render the map
        - [x] Build tileset model in memory
        - [x] Build tilemap model in memory
        - [x] Create the tilemap in World (physics). This might be able to be one large static body
            - [x] We'll go naive first. Will likely need to group tiles and make separate bodies. I want to rely on
              the tile collisions themselves because slopes will make more sense that way.
        - [x] Render 