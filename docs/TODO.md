# Evolving TODO list

I'm trying to manage the entire project within GitHub, with things living alongside the source code. 
Hopefully, this will make it easier to document everything and rarely have to leave my IDE -- while also giving the flexibility
to share my work and musings with others.

## The List
<i>Last Updated: 2025-01-11</i>

- [ ] Binary Packing
  - [ ] Pack a single integer
  - [ ] Pack an arbitrary number of integers
    - The point of these two is to work out a (hopefully) elegant interface for creating serializable types
    -- This is also more metaprogramming practice
- [ ] Use Cereal for binary cerealization of my network types
  - git@github.com:USCiLab/cereal.git
- [ ] Collision
  - [x] Create new cmake project somewhere in the repository, ozzCollisionLib.
    - [x] It will use glm as its math library
    - [x] Google tests
  - [x] Create shape types
    - [x] Point
    - [ ] Circle
    - [ ] Rectangle
    - [ ] Convex Polygon
  - [ ] Develop collisions using some form of TDD.
    - [ ] Fill out the rest of this
   

- [ ] Single player core movement
  - [ ] axis values in the input system
  - [ ] Move
  - [ ] Jump
  - [ ] Walk up and down slopes
  - [ ] Slide down slopes
   
   
- [ ] Tilemap Objects
  - [ ] Spawn points / spawn zone
   
   
- [ ] Network MVP
  - [ ] Player network statemachines 
  - [ ] Players spawn on server, replicate to clients
  - [ ] Player movement replicates
  - [ ] Players see each other disconnect
   
   
- [ ] Unit Tests?
  - I just had the thought that it might be wise to put some unit testing somewhere
  up in this bitch. Especially with the physics stuff I'm working on.