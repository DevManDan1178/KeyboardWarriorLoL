# KeyboardWarriorLoL

KeyboardWarriorLoL is a real-time League of Legends companion application that detects in-game events and makes context-aware chat macros available through configurable global hotkeys.

The application combines a polling-based event processing architecture, real-time game-state integration, configurable input handling, UI overlays, and persistent user configuration into a lightweight desktop application.

### Supporting two types of messages:

* **Default Messages** — Always available and can be sent at any time.
* **Event Messages** — Temporarily available after the detection of specific in-game events (different messages configurable per event).

<br>

*Wait... isn't this just a chat macro?*

*Well yes, but specifically for LoL so it's like a bajillion times cooler and easier to use and stuff!*

## How to Setup
- Click on `Releases`
- Download a release (ZIP)
- Unzip it somewhere you can remember
- Launch the application by clicking on the `KeyboardWarriorLoL.exe` file in the unzipped folder
  - If it is blocked by windows defender, you will have to allow it to run

## Features

* Configurable hotkeys for preset chat messages.
* Sending messages quick without typing.
* In-game Event-aware messaging system.
* Custom titles for every message slot.
* Support for any amount of message and hotkey slots.
* Lightweight and easy to configure.

## Message Types

### Default Messages

Default messages are always available.

Example:

* `gj team`
* `group for dragon`
* `wait for me dont fight`

Pressing the assigned hotkey immediately sends the configured message.

### Event Messages

Event messages are tied to specific game events.

An event's messages become available temporarily after the event occurs in game. 

When available, pressing the corresponding hotkey sends the configured message.

## Events and Event Conditions

`Credit` - Only fires if the credit is yours

`Contribution` - Fires if the credit is yours or if you assisted

`Same Team` - Fires if you are on the scoring team

`Automatic` - No special conditions

- Game State
  - Game Start - `Automatic`
    
- Kills
  - First Blood - `Credit`
  - Assisted Kill - `Credit`
  - Solo Kill - `Credit`
  - Double Kill - `Credit`
  - Triple Kill - `Credit`
  - Quadra Kill - `Credit`
  - Pentakill - `Credit`
  - Ace - `Same Team`
  - Death - `Credit`
    
- Objectives 
  - Dragon - `Contribution`
  - Baron - `Contribution`
  - Rift Herald - `Contribution`
  - Void Grubs - `Contribution`
  - Atakhan - `Contribution`
    
- Structures
  - First Turret - `Credit`
  - Turret - `Contribution`
  - Inhibitor - `Contribution`


#### Some Information About How Events Work
- Events are stored in a queue, where the current event can be skipped in favor of the next one

- **Kill events** (excluding `Ace` and `Death`) will automatically be placed first in the queue (erasing all previous events).
  - Kill streak events will automatically update to the most recent streak. (Ex: getting a triple kill will automatically set the current event to `Triple Kill`)
  - Getting a kill with an event already active will set the kill as the active event instead of placing it after

- The **Quadra Kill** event expires after 30 seconds (the maximum possible amount of time before the timer for a pentakill expires)


### In-Game Workflow
The In-game Overlay UI can be toggled between showing only on events or always showing.

When an event is detected, the overlay UI will show the current event, the messages and corresponding hotkeys, the pending next event (if there is one), and a progress bar indicating the time left for the event.

Pressing a hotkey will type the message in the chat. 

\***The message is only properly typed if the chat is initially closed**\*

## Message Titles

Each message slot includes a customizable title.

Titles are used only within KeyboardWarriorLoL to help identify messages when configuring or viewing hotkey assignments. They do not affect the text sent to the in-game chat.

This allows players to quickly distinguish between and remember their preset messages.

## Configuration

Each message slot can be configured with:

* Title
* Message Content

The message content is what will be typed into the chat bar, so start the message with `/All` to type in All Chat.

Message sizes are limited to the same size as LoL in-game chats *(127 characters long)*.

Example:
```text
Default Message 1
Title = "GLHF"
Content = "/All Good luck have fun"
```

Hotkey slots can be configured by clicking the hotkey button and pressing a valid hotkey.

Example:

```text
Default Hotkey 1
[Mouse 5]   [Delete Hotkey]
```

## Usage
0. (Optional) Create a shortcut for the KeyboardWarriorLoL.exe executable in the installed folder.
1. Launch League of Legends alongside KeyboardWarriorLoL (KeyboardWarriorLoL.exe).
2. Configure your message titles, messages, hotkeys, and mod configurations (if needed).
3. Test it in practice tool (get used to it).
4. Have fun with it.

*Do not modify any files in the application's folder.*

Please use this responsibly.
- Don't use this to spam the chat.
- Treat people with respect.
- Follow Riot Games' rules and community guidelines.

## Screenshots
### Configuration UI Snippet
<img width="305" height="320" alt="image" src="https://github.com/user-attachments/assets/76601b06-8bf4-49d8-9756-00d95c2cfde8" />


### In-game Overlay UI
<img width="414" height="140" alt="image" src="https://github.com/user-attachments/assets/2bb60662-a19a-439a-9d51-7ec34469b67a" />


### In-game Example: Sending Message
<img width="250" height="25" alt="image" src="https://github.com/user-attachments/assets/9c9f1f24-9ded-4fcd-888f-ff866e3cac4a" />


## Disclaimer !

I have reached out to Riot Games Support by filing a ticket in the `Discuss Personal Suspension or Restriction` section and asking if this application was allowed.

Their response has neither stated it as being allowed nor stated it as being against terms of service. 

You should be fine as long as you use it responsibly *(use at your own risk)*.

*KeyboardWarriorLoL is an unofficial third-party tool and is not affiliated with or endorsed by Riot Games. Use of third-party software may violate Riot Games policies or Terms of Service. Users assume all risk associated with using this software, including potential account penalties.*

# Technical Overview
## How It Works
KeyboardWarriorLoL uses a polling-based event-driven workflow to monitor the League of Legends client and process new game events.

A dedicated worker thread continuously checks the state of the League client and adjusts its polling frequency depending on the current state:

- **Not in game** - The client is checked every 2 seconds.
- **Loading screen** - The client is checked every 500 ms.
- **In game** - Game events are polled every 100 ms.

Once the application detects that a game is active, it retrieves the current player and team information and begins polling the Live Client Data API for new events.

```
League Client
     |
     v
Check Client State
     |
     +-- Not in game
     |      |
     |      +-- Check every 2 seconds
     |
     +-- Loading
     |      |
     |      +-- Check every 500 ms
     |
     +-- In game
            |
            +-- Get player/team information
            |
            +-- Poll events every 100 ms
                    |
                    v
              New Game Events
                    |
                    v
              Event Handler
                    |
                    +-- Validate event
                    +-- Determine player involvement
                    +-- Convert to application event
                    +-- Add to event queue
                    |
                    v
              Active Event
                    |
                    +-- Overlay
                    +-- Hotkeys
                    +-- Expiration
```

### Event Polling
The Live Client Data API provides an event list along with an `EventID` for each event.
KeyboardWarriorLoL keeps track of the most recently processed event ID and requests events starting from that ID. 
Each returned event is then checked against the current ID before being processed, preventing the same event from being handled multiple times.

This allows the application to continuously poll the API without requiring a persistent event subscription.

### Event Processing
Raw League events are passed to `LoLEventHandler`, which determines whether the event is relevant to the local player and converts it into one of KeyboardWarriorLoL's application events.

For example:

- A `ChampionKill` event can become `Death`, `Solo Kill`, or `Assisted Kill`.
- A `Multikill` event can become `Double Kill`, `Triple Kill`, `Quadra Kill`, or `Pentakill`.
- Objective events such as `DragonKill` are only processed when the local player received credit or assisted.
- Team-based events such as `Ace` are checked against the local player's team.

Events that do not have configured messages are ignored.

### Event Queue
Once an event passes its conditions, it is added to the event queue.

Normal events are appended to the queue. 
Higher-priority events, such as kills and kill streaks, can instead replace the existing queue and become the active event immediately.

The event handler tracks the start time of the current event and periodically checks whether it has expired. 
When an event expires, it is removed from the front of the queue and the next pending event becomes active.

This separates the process of detecting game events from deciding which event should currently be presented to the user.

```
League API
    |
    v
Poll for new events
    |
    v
Validate EventID
    |
    v
Process raw event
    |
    v
Check player/team involvement
    |
    v
Convert to application event
    |
    v
Add to event queue
    |
    +----------------------+
    |                      |
    v                      v
Current Event         Pending Events
    |
    v
Overlay + Hotkeys
    |
    v
Event Expires
    |
    v
Next Event
```

## Tech Stack

- **Language:** C++20
- **Build System:** CMake
- **UI:** Dear ImGui
- **Windowing / Input Integration:** SDL2
- **Global Input:** libuiohook
- **Graphics:** OpenGL
- **Game Integration:** League of Legends Live Client Data API
- **Configuration:** JSON
- **HTTP:** cpp-httplib / libcurl

The project is built as a native Windows desktop application using C++20 and CMake.

## Limitations

- `Alt` and `CTRL` keys are not supported due to their implementation on the LoL client.
    - `Alt + Enter` will toggle fullscreen, so it can absolutely not be used with sending a message
    - `CTRL` will block opening the in-game chat with `Enter`
- `Enter` key is not supported as it risks disrupting the application's process of message sending.
    - The application sends in-game messages by inputting `Enter`, `Your Message` character by character, and `Enter`
    - By pressing `Enter` before this, the chat is open then closed before the message is typed, then reopened
- No `GameEnd` event on nexus destruction (rip).
    - Unfortunately, the client's API's `GameEnd` event fires after the victory screen, so it is basically useless.
    - Besides that, there are no reliable ways to detect a game ending.
- Players with the same name cannot be distinguished.
    - The LoL client API for fetching game events only provides events with the summoner name (without the tagline).
    - The application might attribute events belonging to another player with the same name to the local player.

## Architecture
The source code is separated into several components based on responsibility:

```text
src/
├── core/
│   └── Application lifecycle and entry point
│
├── lol/
│   ├── League client/game-state integration
│   ├── Event processing
│   └── Chat message sending
│
├── input/
│   ├── Global input handling
│   └── Hotkey conversion
│
├── managers/
│   ├── Hotkey management
│   └── Message management
│
├── ui/
│   ├── Configuration UI
│   ├── Hotkey UI
│   └── Core/overlay UI
│
└── helpers/
    └── Shared utilities and HTTP functionality
```

The main responsibilities are separated so that game integration, event processing, input handling, configuration, and presentation do not need to be implemented in a single component.

For example, `LoLReader` is responsible for communicating with the League client, while `LoLEventHandler` interprets the returned game events. `LoLChatSender` handles sending messages, while the input and manager components handle configurable hotkeys and messages.

This separation also makes the event-processing logic independent from the UI that displays the resulting event.

## Engineering Challenges
### Polling a Real-Time Game Client
The League client does not provide the application with a persistent event subscription for the events it needs.

KeyboardWarriorLoL therefore uses a dedicated worker thread to continuously poll the client.

The polling interval changes depending on the current game state:

- **Not in game:** 2 seconds
- **Loading:** 500 ms
- **In game:** 100 ms

This reduces unnecessary requests when the application is idle while still providing responsive event detection during gameplay.

### Preventing Duplicate Events
The Live Client Data API provides an ID for each game event.

The application keeps track of the most recently processed event ID and only processes events that have not already been handled.

This allows the application to continuously poll the event endpoint without processing the same game event multiple times.

### Interpreting Raw Game Events
The events returned by the League client do not always directly correspond to the events shown to the user.

For example, a raw `ChampionKill` event must be interpreted to determine whether it represents:

- A death
- A solo kill
- An assisted kill

Similarly, multikill events need to be converted into the appropriate application event such as `Double Kill`, `Triple Kill`, `Quadra Kill`, or `Pentakill`.

The event handler also checks whether the local player received credit, contributed to the event, or belongs to the relevant team before creating an application event.

### Managing Simultaneous Events
Multiple game events can occur while another event is already being displayed.

The event queue allows the application to preserve pending events while giving higher-priority events control of the active state.

Kill events are given priority because they are time-sensitive and can change rapidly as a kill streak progresses.

For example, a `Double Kill` followed by a `Triple Kill` should not result in both events being displayed independently. The active kill streak is updated to the most recent streak.

### Real-Time Input
Sending a message requires interacting with the League of Legends chat through simulated keyboard input.

The application therefore needs to coordinate global hotkey detection with the sequence used to open the chat, type the message, and submit it.

This introduces restrictions around keys such as `Enter`, `CTRL`, and `Alt`, which are documented in the limitations section.

## Design Decisions
### Polling Instead of a Persistent Event Subscription
The application uses polling because the required League client events are exposed through the Live Client Data API as an event list.

Event IDs are used to keep track of which events have already been processed.

### Adaptive Polling
The application does not need to poll the League client at the same rate at all times.

Polling is intentionally slower when the user is not playing and becomes more frequent during loading and active gameplay.

This keeps the application lightweight while maintaining responsive event detection.

### Queue-Based Event Handling
Events are placed into a queue instead of simply replacing the current event.

This allows events that occur close together to be preserved while still supporting priority rules for time-sensitive events such as kills and kill streaks.

### Separate Event Detection from Presentation

The League client integration and event processing do not directly control how an event is displayed.

An event is first detected and converted into an application-level event before being consumed by the UI and hotkey systems.

This keeps the event-processing logic separate from the presentation layer.

