# Evolving TODO list

I'm trying to manage the entire project within GitHub, with things living alongside the source code. 
Hopefully, this will make it easier to document everything and rarely have to leave my IDE -- while also giving the flexibility
to share my work and musings with others.

## The List
<i>Last Updated: 2024-12-31</i>

- [x] World
  - [x] Gameobjects and ECS?
- [x] Pick a 2D Physics Engine
  - [x] Integrate the engine
  - [ ] Create a player
  - [ ] Move the character
  - [x] Player can jump
  - [ ] Camera Follows Player
- [ ] Use Tiled to create initial world.
  -  [x] Read Tiled-generated map (https://github.com/SSBMTonberry/tileson)
  -  [ ] Build / render the map
    - [ ] Build tileset model in memory
    - [ ] Build tilemap model in memory
    - [ ] Create the tilemap in World (physics). This might be able to be one large static body
      - [ ] We'll go naive first. Will likely need to group tiles and make separate bodies. I want to rely on
            the tile collisions themselves because slopes will make more sense that way.
    - [ ] Render 
- [ ] Player spawns into map
  - [ ] Player does not fall through floor
  - [ ] Player can move around on map
- [ ] Network the players
  - [ ] Players can spawn on map
  - [ ] Players see each other move / jump
  - [ ] Players see each other disconnect