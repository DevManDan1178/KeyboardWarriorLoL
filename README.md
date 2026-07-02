# KeyboardWarriorLoL

KeyboardWarriorLoL is a League of Legends companion application that allows players to send predefined chat messages using configurable hotkeys. 
Messages can be triggered at any time or unlocked dynamically based on detected in-game events such as kills and the takedown of objectives and structures.

The mod supports two types of messages:

* **Default Messages** — Always available and can be sent at any time.
* **Event Messages** — Unlocked by specific in-game events and can only be sent after the corresponding event has been detected (for a limited duration).

## How to Setup
- Click on `Releases`
- Download a release (ZIP)
- Unzip it somewhere you can remember
- Launch the application by clicking on the `KeyboardWarriorLoL.exe` file in the unzipped folder
  - If it is blocked by windows defender, you will have to allow it to run

## Features

* Configure hotkeys for commonly used chat messages.
* Send messages quick without typing.
* Event-aware messaging system.
* Custom titles for every message slot.
* Support for multiple event-triggered message slots.
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

Event messages are tied to game events and are identified by an index.

An event message becomes available only after its associated event has occurred. 

Once available, pressing the assigned hotkey sends the configured message.

Supported events include:

- Game State
  - Game Start
    
- Kills
  - First Blood 
  - Assisted Kill
  - Solo Kill
  - Double Kill
  - Triple Kill
  - Quadra Kill
  - Pentakill
  - Ace
  - Death
    
- Objectives
  - Dragon
  - Baron
  - Rift Herald
  - Void Grubs
  - Atakhan
    
- Structures
  - First Turret
  - Turret
  - Inhibitor

#### Some Information About How Events Work
- Events are stored in a queue, where the current event can be skipped in favor of the next one

- **Kill events** (excluding First Blood, Ace and Death) will automatically be placed first in the queue (erasing all previous events).
  - Kill streak events will automatically update to the most recent streak. (Ex: getting a triple kill will automatically set the current event to `Triple Kill`)
  - Getting a kill with an event already active will set the kill as the active event instead of placing it after

- The **Quadra Kill** event expires after 30 seconds (the maximum possible amount of time before the timer for a pentakill expires)


### In-Game Workflow
The In-Game Overlay UI can be toggled between showing only on events or always showing.

When an event is detected, the overlay UI will show the current event, the hotkeys with the corresponding messages (by title), and a pending next event (if there is one).

Pressing a hotkey will type the message in the chat. 

\***The message is only properly typed if the chat is initially closed**\*

## Message Titles

Each message slot includes a customizable title.

Titles are used only within KeyboardWarriorLoL to help identify messages when configuring or viewing hotkey assignments. They do not affect the text sent to in-game chat.


This allows players to quickly distinguish between message slots without needing to read the full chat message when properly configured.

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

*Do not modify the other files in the mod's folder.*

Please use this mod responsibly.
- Don't use this to spam the chat.
- Treat people with respect.
- Follow Riot Games' rules and community guidelines.


## Limitations

- `Alt` and `CTRL` keys are not supported due to their implementation on the LoL client.
    - `Alt + Enter` will toggle fullscreen, so it can absolutely not be used with sending a message
    - `CTRL` will block opening the in-game chat with `Enter`
- `Enter` key is not supported as it risks disrupting the mod's process of message sending.
    - The mod sends in-game messages by inputting `Enter`, `Your Message` character by character, and `Enter`
    - By pressing `Enter` before this, the chat is open then closed before the message is typed, then reopened
- No `GameEnd` event on nexus destruction (rip).
    - Unfortunately, the client's API's `GameEnd` event fires after the victory screen, so it is basically useless.
    - Besides that, there are no reliable ways to detect a game ending.

## Disclaimer

I have tried reaching out to Riot Games by filing a support ticket in the `Discuss Personal Suspension or Restriction` section and asking if this mod was allowed.

Their response has neither stated it as being allowed nor stated it as being against terms of service. 

You should be fine as long as you use it responsibly *(use at your own risk)*.

*KeyboardWarriorLoL is an unofficial third-party tool and is not affiliated with or endorsed by Riot Games. Use of third-party software may violate Riot Games policies or Terms of Service. Users assume all risk associated with using this software, including potential account penalties.*

