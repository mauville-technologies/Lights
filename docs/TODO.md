# Evolving TODO list

I'm trying to manage the entire project within GitHub, with things living alongside the source code. 
Hopefully, this will make it easier to document everything and rarely have to leave my IDE -- while also giving the flexibility
to share my work and musings with others.

## The List
<i>Last Updated: 2025-01-11</i>

- [ ] Collision
  - [x] Create new cmake project somewhere in the repository, ozzCollisionLib.
    - [x] It will use glm as its math library
    - [x] Google tests
  - [x] Create shape types
    - [x] Point
    - [x] Circle
    - [x] Rectangle
    - [ ] Convex Polygon
  - [x] Develop collisions using some form of TDD.
    - [x] Fill out the rest of this
  - [ ] OzzWorld2D
    - [x] Gravity
    - [ ] Ground collision
    - [ ] Wall Collision
   
- [ ] Sprite class (core game object)
  - [ ] Nested sprites
    - [ ] Proper transform nesting ( Likely implemented as part of GameObject)
    - [ ] Rendering nested sprites ( implemented as part of Sprite)
  - [x] Add bodies to sprite ( this might be a GameObject thing as well)
  - [x] Debug draw bodies on sprite ( these should be a few sceneobjects statically loaded that can be copied and rendered at will)

- [ ] Single player core movement
  - [ ] axis values in the input system
  - [ ] Move
  - [ ] Jump
  - [ ] Walk up and down slopes
  - [ ] Slide down slopes
   
   
- [ ] Tilemap
  - [ ] Probably a full re-write utitlizing Sprite class
  - [ ] Spawn points / spawn zone
   
   
- [ ] Network MVP
  - [ ] Player network statemachines 
  - [ ] Players spawn on server, replicate to clients
  - [ ] Player movement replicates
  - [ ] Players see each other disconnect
   
   
- [ ] Unit Tests?
  - I just had the thought that it might be wise to put some unit testing somewhere
  up in this bitch. Especially with the physics stuff I'm working on.