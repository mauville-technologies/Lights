# Dev Logs

For the purposes of maintaining a train of thought across multiple days and through the large amount of 
context switching that I do when it comes to development, I've been working on a habit of keeping dev logs
in markdown form in one of my work repositories. 

This was originally an extension of the work flow I've developed in this repository. Now I will inherit that back
as I am finding that since this is not my professional endeavour (yet) I'm having trouble remembering the minutae of what
I was immediately about to work on "next".

I'll keep newest at the top, with each header being some arbitrary period of time to keep track of roughly when
I was doing what.

## 18-02-2025 - 2 days to Victoria
Well -- my flight to Victoria got cancelled due to snow and my trip has moved the Thursday. This is unfortunate and relevant here as I had plans for the plane
and my time over there for this project. That being said, it's probably for the best that I didn't get to it yet. I ended up doing something different than
I had in mind because I didn't like some of the structure I had going on. Most of that is fixed now and I've made a few interfaces much better and less annoying to 
work with.

The next thing I want to work on is:
- Adding bodies to my sprites, specifically Rectangles and circles
- Drawing those debug shapes correctly overtop the new Sprite class.
- Getting basic gravity and ground collision working. 

These three things will probably take up enough of my time to be getting on with for now. I can decide where I want to go from there after that.

## 10-02-2025 - One week pre-victoria

So I've decided that I need to start getting back on the Lights train. With my "trip" to Victoria coming up next week, I feel that it's the perfect 
opportunity to spend maybe an unhealthy amount of time programming some stuff. 

I dont want to waste all my dev energy on logging so here's my current priority:

- Basic world system. Essentially physics. It will have a tick with a way to create and remove bodies. A lot of this work lives in CollisionSystem at the moment.
I'll move that over and use that as a base.

## 30-01-2025 Status Update, some thoughts I want to write down.

I haven't made a whole lot of progress on the collision system for this. It's been a busy few weeks and I've been a bit behind
at work and at home. I will be attempting to get back on track -- but FFVII Rebirth has got me in its clutches at the moment. I need to 
pull back a bit on it now that I'm back at work.

Anyways, the communication between client and server in this project is purely serialized and deserialized binary data.
It's currently a very adhoc system which is prone to mistakes and is kind of annoying to extend. I have needs to serialize
some stuff at work as well for a personal project dev environment thing. I usually stick with something like a json serializer for these kinds of
things but I remembered the pain I had over here. Therefore, I think it could be worth my time to build out a binary format
that can output a stream of bytes. This stream can then be piped over the network or streamed in and out of files.

I haven't done anything like that before but it seems like an interesting little utility that would be very handy to have
for things like save systems, streaming data over a network, and other such and things. I've always wanted to try and build something more elegant than these
gross json and the like parsers. DOMAIN SPECIFIC LANGUAGE BIATCH!

---
After doing some research and thinking, it's probably not a great idea to write my own binary serializer.
The research turned up Cereal, which can do binary JSON and XML. I'm going to add "use cereal to parse data" to the TODO list.
Well... we'll see but I have options.

## 23-01-2025 Collision ctd.
I had to take some time off on this project for a little bit because I haven't been finding enough other time
to take care of work things. I'm returning today sitting on the couch with my new wireless setup
hopefully so the little one can get used to me working while taking care of him..

For some reason though whenever he gets too close the wireless signal on this keyboard starts cutting out and causing
me to have to wait for him to get distracted so I can get stuff done.

My goal is to continue the collision library today. From what I've gathered in the last few minutes... it seems like I essentially
finished circles(though I remember now that I was trying to calculate circle x circle intersection points for some reason. Going to abandon that and 
simply focus on the basic collisions still.)

Rectangles and polygons are next.

## 14-01-2025 - Collision

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
  - [x] Circle
  - [ ] Rectangle
  - [ ] Convex Polygon
- [ ] Develop collisions using some form of TDD.
  - [ ] Fill out the rest of this
