# Network Communications and General Application flow

The server is going to be as authoritative as possible and hold all game state within its cache, with some form of game state 
backing up happening every so often. This backup process will be thought of later.

As of writing, the application / connection flow looks like this:

```mermaid
flowchart LR
    Disconnect-->Connect
    Connect--
        There will be a
        time limit
        enforced on login
    -->LoggedIn
    LoggedIn-->Disconnect
    Connect-->Disconnect
```
Additionally, the current state of the application contains no "game". The client at the moment is simply 2 quads 
rendered locally on a cornflower blue background. One of these quads can be moved with the arrow keys <span style="font-size:10px;vertical-align:top">[commit](https://github.com/Ozzadar/Lights/commit/9a8a0a4cf3b93f69030b7e37261d31f074284f36)</span>

The purpose of this document is essentially to brainstorm what the server and client statemachines might look at -- ultimately
landing on a rough first draft of the statemachines and messages that will be exchanged between the server and client.

## Client State

The client itself will probably have a couple "application level" state machines.

1. Network connection state (Connected, Connecting, Connected, LoggedIn)
2. Application Screen State (Login Screen, Main Menu Screen / Character Select, Loading, Game Screen)
3. Game State [within the game screen]. (TBD as game not yet designed)

```mermaid
---
        title: Network Connection State
---
stateDiagram-v2
    [*]-->Disconnected
    Disconnected-->[*]
    
    Disconnected-->Connected
    Connected-->Disconnected
    Connected-->LoggedIn
    LoggedIn-->Disconnected
    
    note right of LoggedIn
        When in LoggedInState, the application is past the login screen
    end note
```

```mermaid 
---
        title: Application State
---
stateDiagram-v2
    state "Login Screen" as Login: Disconnected
    state "Menu Screen" as Menu: Connected + Logged In
    state "Loading" as Loading: From here on, network state doesn't\n really change. Received game state.
    state "Game" as Game: The main game scene. In this scene, the client essentially\nfor world update events.
    [*]-->Login
    Login-->[*]
    
    Login-->Menu
    Menu-->Login
    Menu-->Loading
    Loading-->Login
    Loading-->Game
    Game-->Menu
    Game-->Login
```

```mermaid
---
    title: Game States
---
stateDiagram-v2
    state "Playing" as Playing: Playing the game

    note left of Playing
        For purposes of this document, the game only has one 
        state. The game screen states are a different beast that
        I'll figure out as I flesh out the game idea.
        
        This state might also be dictated by the server (see below)
    end note
    [*]-->Playing
```

## Server Things

The way the server is currently architected, it will asynchronously accept new connections while running the main server loop on another.
This is very similar to the way that client is set up but without the "screen states", and the "network states" being simplified down to "Waiting for Connections".

Therefore for purposes of this discussion there are 2 main state machines:

1. Central Ticking "Game" State Machine
2. Player State Machine (essentially the client's Network State machine, augmented with some more game specific states)
3. Player Game State

```mermaid
---
    title: Player State Machine
---
stateDiagram-v2
    [*]-->Disconnected
    Disconnected-->[*]
    Disconnected-->WaitingForAuthentication
    WaitingForAuthentication-->Authenticated
    Authenticated-->EnteringGame
    EnteringGame-->InGame
    
    WaitingForAuthentication-->Disconnected
    Authenticated-->Disconnected
    EnteringGame-->Disconnected
    InGame-->Disconnected
    
    note left of WaitingForAuthentication
        The Server will forcibly close the
        connection if authentication is not
        performed within a prescribed time
        limit
    end note

    note right of InGame
        There's likely to be a "Player GameState" in play
        around this point but for now, as with the client,
        we'll go with a single "in game" state
    end note
```

```mermaid
---
    title: Player Game State
---
stateDiagram-v2
    [*]-->Playing
    Playing-->[*]
```

```mermaid
---
    title: Game state (names need work)
---

stateDiagram-v2
    state "Simulating / Accepting Connections" as Simulating
    [*]-->Uninitialized
    Uninitialized-->Initializing
    Initializing-->Simulating
    Simulating-->ShuttingDown
    ShuttingDown-->Uninitialized
    
    note left of Simulating
        Everything essentially happens in here. ShuttingDown will send
        ServerLeaving messages but otherwise the meat and potaters of
        application is here.
    end note
```

## MESSAGES! THE WHOLE REASON THIS DOCUMENT EXISTS

Laying out the various states for everything was all in preparation for starting to formalize the messages that are required
to enable these systems. So here's my attempt at a sequence diagram attempting to define some of these messages at the highest of levels.

Hopefully, mermaid will let me section off various sections to overlap with different states I described above.

```mermaid
---
    title: Messages
---
sequenceDiagram
%%    autonumber
    participant C as Client
    participant S as Server
    
    %% Client Connect and Authenticate
    rect rgb(25,25,25)
        activate C
        critical Connect to Server
            C-->S: Connect
        end
        note over C, S: The Server will disconnect the client <br/>if authentication isn't performed <br/> quickly
        C->>S: AuthenticationRequest
        deactivate C
        
        activate S
        alt success
        S->>C: AutenticationSuccessful
        else failure
        S->>C: AuthenticationFailed
        S-->C: Disconnect
        deactivate S
        end
    end
    
    rect rgb(70,50,50)
        activate C
        C->>S: EnterGame
        deactivate C
        
        activate S
        par Send initial state
            S->>C: InitialPlayerState
            S->>C: NearbyPlayerStates
            and
            S->>C: WorldState
        end
        deactivate S
    end

    rect rgb(50,70,50)
        par In Game Loop
            loop ServerTicking 
                S->>C: UpdateWorldState
                S->>C: UpdatePlayerState
                S->>C: UpdateNearbyPlayerStates
            end
        and
            loop ClientTicking
                C->>S: PlayerInput
            end
        end
    end
```