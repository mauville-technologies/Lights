# Collision and Physics

As of today (Jan 11th 2025), I have box2D integrated to the point that my pepe character can interact with a tilemap created
in Tiled and loaded through here. The issue is that I'm not really digging box2D that much. I think most of what I need to
accomplish the type of game that I want to make can be done myself from the ground up using a couple of basic collision shapes
and my own custom collision engine.

This may be bad to say, but I don't think it will be *too* hard to get something basic up and running.

This document is my brainstorming on how this system will work. I suspect integration with the gameobjects themselves will be done
in a similar vein to how I did Box2D. 

Here are some goals of the system:

1. Separate "PhysicsTick" that objects implement to perform kinematic movement
   1. This PhysicsTick will also be where collisions are detected and handled.
   2. Objects can be marked static to disable the PhysicsTick function. Static colliders will only affect
   objects during dynamic collider resolution.
2. There should be a simple way to get list of potentially colliding objects.
3. Floating data store wherein global physics concepts such as "gravity" can be stored and queried. Objects performing their
"PhysicsTick" can use these values however they see fit.
4. Colliders should be taggable (Ground, Player, Projectile, etc) in order to customize behaviour
5. Collision Shapes needed for first pass:
   1. Quads
   2. Circles
   3. convex polygons (for sloped ground, specifically)
   4. points