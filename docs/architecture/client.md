# Client Architecture

It's getting to the point where I"m having a little trouble remembering how everything is called. Therefore, let's create a flowchart that 
will hopefully do a decent job a demonstrating how things flow.

As of this writing, the client and server are mostly separate concepts with the client building out the main
game loop that will be generalized and/or moved to run on the server. Therefore, it's important that I remember how
this is set up. Writing it out might also give me some insight into what I could be missing here.

## Main Loop
```mermaid
flowchart TD
    subgraph Game["&nbsp;"]
        subgraph Init["&nbsp;"]
            direction TB
            initInput-->initWindow-->initGL-->initRenderer-->initUI-->initScene-->initNetwork
        end
        subgraph GameLoop
            subgraph SceneTick["SceneTick(deltatime)"]
                subgraph LayersTick["LayersTick(deltatime)"]
                    goTick("GameObject::Tick()")
                end
                subgraph LayersTick2["LayersTick(deltatime)"]
                    goTick2("GameObject::Tick()")
                end
                subgraph WorldPhysicsTick
                    
                end
                WorldPhysicsTick-->LayersTick
                WorldPhysicsTick-->LayersTick2
            end
            SceneTick --> PossiblyRender --> ReadInput -->  SceneTick
        end
        gamerun("OZZ::Game::Run()") --> Init --> GameLoop
        
    end
    main.cpp --> Game 
    
```