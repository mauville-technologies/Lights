# Dev Logs

For the purposes of maintaining a train of thought across multiple days and through the large amount of 
context switching that I do when it comes to development, I've been working on a habit of keeping dev logs
in markdown form in one of my work repositories. 

This was originally an extension of the work flow I've developed in this repository. Now I will inherit that back
as I am finding that since this is not my professional endeavour (yet) I'm having trouble remembering the minutae of what
I was immediately about to work on "next".

I'll keep newest at the top, with each header being some arbitrary period of time to keep track of roughly when
I was doing what.

## 14-01-2025 - Collisions

So as mentioned in the physics brainstorming page, I'm rolling my own collision for this project because it's probably
simple enough to put together the things I want to do. I'm starting on this endeavour in earnest today.

I want to do this right enough so that I don't need to re-tweak this all the time. That being said, it is going to be my
first real "subproject"; meaning it will be its own lib. So that means I need to do all the busywork of setting up the
project structure. Despite my better judgement, this library will also have unit tests for testing the collisions. 
This just makes sense really as otherwise I'll have to be eyeballing things all the time and I make enough mistakes when 
doing rendering things that it would be ill-advised.

So here's the current TODO list:

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
