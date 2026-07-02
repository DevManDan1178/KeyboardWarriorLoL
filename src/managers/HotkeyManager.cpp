#include "../external/json.hpp"
#include "managers/HotkeyManager.hpp"
#include "input/HotkeyConverter.hpp"
#include <uiohook.h>
#include <unordered_map>
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <string>


#define SDL_MAIN_HANDLED
#include <SDL.h>


using json = nlohmann::json;





static std::filesystem::path getExeDir()
{
    std::wstring buffer(32767, L'\0');

    DWORD len = GetModuleFileNameW(
        NULL,
        buffer.data(),
        static_cast<DWORD>(buffer.size())
    );

    if (len == 0)
    {
        return std::filesystem::current_path(); // fallback
    }

    buffer.resize(len);

    return std::filesystem::path(buffer).parent_path();
}

static std::filesystem::path getConfigPath()
{
    return getExeDir() / "config" / "hotkeys.json";
}

HotkeyManager::HotkeyManager(MessagesManager& _messages)
: messages(_messages) {}

static Hotkey parseHotkeyData(json hotkeyData) {
    Hotkey hotkey;
    hotkey.keyCode = hotkeyData.value("Key", 0u);
    std::string bindType = hotkeyData.value("BindType", "");
    hotkey.bindType = bindType == "Keyboard" ? BindType::Keyboard : (bindType == "Mouse" ? BindType::Mouse : BindType::None);
    hotkey.modifiers = hotkeyData.value("Modifiers", 0u);
     return hotkey;
}

static json hotkeyToJson(const Hotkey& hotkey) {
    return {
        {"Key", hotkey.keyCode},
        {"BindType",
            hotkey.bindType == BindType::Keyboard ? "Keyboard" :
            hotkey.bindType == BindType::Mouse ? "Mouse" :
            "None"},
        {"Modifiers", hotkey.modifiers}
    };
}

bool HotkeyManager::load() {
    std::ifstream file(getConfigPath());
    if (!file.is_open()) {
        std::cout << "Failed to open file at path " << getConfigPath() << std::endl;
        return false;
    }
    
    json hotkeysData;
    file >> hotkeysData;
 
    //Get default keybinds
    json _defaultHotkeys = hotkeysData["Defaults"];
    for (int i = 0; i < _defaultHotkeys.size(); i++) {
        Hotkey hotkey = parseHotkeyData(_defaultHotkeys[i]);
        defaultHotkeys.push_back(hotkey);
    }

    //Get event keybinds
    json _eventHotkeys = hotkeysData["Events"];
    for (int i = 0; i < _eventHotkeys.size(); i++) {
        Hotkey hotkey = parseHotkeyData(_eventHotkeys[i]);
        eventHotkeys.push_back(hotkey);
    }
    
    Hotkey _skipEventHotkey = parseHotkeyData(hotkeysData["SkipEvent"]);
    skipEventHotkey = _skipEventHotkey;


    float _eventHotkeyDuration = hotkeysData["EventHotkeyDuration"];
    eventHotkeyDuration = _eventHotkeyDuration;

    Hotkey _toggleInGameInteractableHotkey = parseHotkeyData(hotkeysData["ToggleInGameInteractable"]);
    toggleInGameInteractableHotkey = _toggleInGameInteractableHotkey;

    Hotkey _toggleInGameAlwaysVisibleHotkey = parseHotkeyData(hotkeysData["ToggleInGameAlwaysVisible"]);
    toggleInGameAlwaysVisibleHotkey = _toggleInGameAlwaysVisibleHotkey;

    return true;
}

void HotkeyManager::attemptWriteToJSON() {
    bool success = writeToJSON();
    if (!success) {
        std::cout << "unable to write hotkeys to JSON" << std::endl;
    }
}

bool HotkeyManager::writeToJSON() {
    try {
        std::ofstream file(getConfigPath());
        if (!file.is_open()) {
            std::cout << "Failed to open file for writing: " << getConfigPath() << std::endl;
            return false;
        }

        json hotkeysData;

        // Defaults
        hotkeysData["Defaults"] = json::array();
        for (const Hotkey& hotkey : defaultHotkeys) {
            hotkeysData["Defaults"].push_back(hotkeyToJson(hotkey));
        }

        // Events
        hotkeysData["Events"] = json::array();
        for (const Hotkey& hotkey : eventHotkeys) {
            hotkeysData["Events"].push_back(hotkeyToJson(hotkey));
        }

        hotkeysData["SkipEvent"] = hotkeyToJson(skipEventHotkey);     
        hotkeysData["ToggleInGameInteractable"] = hotkeyToJson(toggleInGameInteractableHotkey);
        hotkeysData["EventHotkeyDuration"] = eventHotkeyDuration;
        hotkeysData["ToggleInGameAlwaysVisible"] = hotkeyToJson(toggleInGameAlwaysVisibleHotkey);

        file << hotkeysData.dump(4); // pretty-print with 4-space indentation
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error writing hotkeys JSON: " << e.what() << std::endl;
        return false;
    }
}

void HotkeyManager::setEventHotkeyDuration(float duration) {
    eventHotkeyDuration = std::round(std::clamp(duration, minEventHotkeyDuration, maxEventHotkeyDuration) * 10.0f) * 0.1f;
    attemptWriteToJSON();
    return;
}

// Returns true if hotkeys are equal and not of bindType None
static bool checkHotkeysEqual(Hotkey hotkey, Hotkey otherHotkey) {
    return hotkey.bindType != BindType::None && (hotkey.bindType == otherHotkey.bindType && hotkey.keyCode == otherHotkey.keyCode && hotkey.modifiers == otherHotkey.modifiers);
}

static bool checkHotkeyIsInList(Hotkey hotkey, const std::vector<Hotkey> hotkeyList, int exceptForIndex = -1) {
    for (int i = 0; i < hotkeyList.size(); i++) {
        if (i != exceptForIndex && checkHotkeysEqual(hotkey, hotkeyList[i])) {
            return true;
        }
    }
    return false;
}

bool HotkeyManager::setHotkey(Hotkey hotkey, bool isEventHotkey, int index) {
    std::vector<Hotkey> &hotkeyList = isEventHotkey ? eventHotkeys : defaultHotkeys;
    if (index >= hotkeyList.size()) {
        std::cout << "Attempt to set non existent hotkey for " << (isEventHotkey ? "events" : "defaults") << " at index " << index << std::endl;
        return false;
    }
    if (checkHotkeyIsInList(hotkey, hotkeyList, index) || checkHotkeyIsInList(hotkey, isEventHotkey ? defaultHotkeys : eventHotkeys) || checkHotkeysEqual(hotkey, skipEventHotkey) || checkHotkeysEqual(hotkey, toggleInGameInteractableHotkey)) {
        std::cout << "Attempt to set an already existing hotkey" << std::endl;
        return false;
    }
    hotkeyList[index] = hotkey;
    attemptWriteToJSON();
    return true;
}

bool HotkeyManager::addHotkey(Hotkey hotkey, bool isEventHotkey) {
    std::vector<Hotkey> &hotkeyList = isEventHotkey ? eventHotkeys : defaultHotkeys;
    if (checkHotkeyIsInList(hotkey, eventHotkeys) || checkHotkeyIsInList(hotkey, defaultHotkeys) 
    || checkHotkeysEqual(hotkey, skipEventHotkey) || checkHotkeysEqual(hotkey, toggleInGameInteractableHotkey)) {
        std::cout << "Attempt to add an already existing hotkey" << std::endl;
        return false;
    }
    hotkeyList.push_back(hotkey);
    attemptWriteToJSON();
    return true;
}

bool HotkeyManager::removeHotkey(bool isEventHotkey, int index) {
    std::vector<Hotkey> &hotkeyList = isEventHotkey ? eventHotkeys : defaultHotkeys;
    if (index >= hotkeyList.size()) {
        std::cout << "Attempt to remove non existent hotkey for " << (isEventHotkey ? "events" : "defaults") << " at index " << index << std::endl;
        return false;
    }
    hotkeyList.erase(hotkeyList.begin() + index);
    attemptWriteToJSON();
    return true;
}

bool HotkeyManager::setSkipEventHotkey(Hotkey hotkey) {
    if (checkHotkeyIsInList(hotkey, eventHotkeys) || checkHotkeyIsInList(hotkey, defaultHotkeys) 
    || checkHotkeysEqual(hotkey, toggleInGameInteractableHotkey) ||checkHotkeysEqual(hotkey, toggleInGameAlwaysVisibleHotkey)) {
        std::cout << "Attempt to set an already existing hotkey to SkipEvent" << std::endl;
        return false;
    }
    skipEventHotkey = hotkey;
    attemptWriteToJSON();
    return true;
}

bool HotkeyManager::setToggleInGameInteractableHotkey(Hotkey hotkey) {
    if (checkHotkeyIsInList(hotkey, eventHotkeys) || checkHotkeyIsInList(hotkey, defaultHotkeys) 
    || checkHotkeysEqual(hotkey, skipEventHotkey) || checkHotkeysEqual(hotkey, toggleInGameAlwaysVisibleHotkey)) {
        std::cout << "Attempt to set an already existing hotkey to ToggleInGameInteractable" << std::endl;
        return false;
    }
    toggleInGameInteractableHotkey = hotkey;
    attemptWriteToJSON();
    return true;
}

bool HotkeyManager::setToggleInGameAlwaysVisibleHotkey(Hotkey hotkey) {
    if (checkHotkeyIsInList(hotkey, eventHotkeys) || checkHotkeyIsInList(hotkey, defaultHotkeys)
    || checkHotkeysEqual(hotkey, skipEventHotkey) || checkHotkeysEqual(hotkey, toggleInGameInteractableHotkey)) {
        std::cout << "Attempt to set an already existing hotkey to ToggleInGameAlwaysVisible" << std::endl;
        return false;
    }
    toggleInGameAlwaysVisibleHotkey = hotkey;
    attemptWriteToJSON();
    return true;
}


//Alt and Ctrl are invalid because they block chat input in LoL
static bool areInvalidsModifiersHeld()
{
    SDL_Keymod mods = SDL_GetModState();

    return (mods & KMOD_CTRL) || (mods & KMOD_ALT);
}

Hotkey HotkeyManager::queryHotkey()
{
    SDL_Event event;

    while (true)
    {
        while (SDL_PollEvent(&event))
        {
            // Keyboard
            if (event.type == SDL_KEYDOWN)
            {
                
                SDL_Scancode sc = event.key.keysym.scancode;

                // Ignore keys
                if (sc == SDL_SCANCODE_LCTRL  || sc == SDL_SCANCODE_RCTRL ||
                    sc == SDL_SCANCODE_LSHIFT || sc == SDL_SCANCODE_RSHIFT ||
                    sc == SDL_SCANCODE_LALT   || sc == SDL_SCANCODE_RALT ||
                    sc == SDL_SCANCODE_RETURN || areInvalidsModifiersHeld()){
                    continue;
                }

                int key = HotkeyConverter::SDLToUiohook(sc);
                //std::cout << "KEY: " << SDL_GetScancodeName(sc) << std::endl;
                if (key == VC_UNDEFINED) {
                    continue;
                }
                    

                uint8_t mods = 0;
                SDL_Keymod sdlMods = SDL_GetModState();
                
                //if (sdlMods & KMOD_CTRL)  mods |= Modifiers::Ctrl;
                if (sdlMods & KMOD_SHIFT){
                    mods |= Modifiers::Shift;
                } 
                //if (sdlMods & KMOD_ALT)   mods |= Modifiers::Alt;

                return { key, BindType::Keyboard, mods };
            }

            // Mouse
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT ||
                    event.button.button == SDL_BUTTON_RIGHT)
                {
                    continue;
                }

                uint8_t mods = 0;
                SDL_Keymod sdlMods = SDL_GetModState();
                if (sdlMods & KMOD_SHIFT) {
                    mods |= Modifiers::Shift;
                } 
                /*
                ignore alt and ctrl because of unwanted behaviours in the LoL client 
                
                if (sdlMods & KMOD_CTRL)  mods |= Modifiers::Ctrl;            
                if (sdlMods & KMOD_ALT)   mods |= Modifiers::Alt;
                 */
                

                return { (int)event.button.button, BindType::Mouse, mods };
            }

            // Quit
            if (event.type == SDL_QUIT)
            {
                return { -1, BindType::None, 0 };
            }
        }

        SDL_Delay(1);
    }
}
