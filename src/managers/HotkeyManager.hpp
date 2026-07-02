#pragma once
#include "managers/MessagesManager.hpp"
#include "input/Hotkey.hpp"
#include <string>
#include <unordered_map>
#include <vector>



 
class HotkeyManager {
    public:       
        const float maxEventHotkeyDuration = 15.0f;
        const float minEventHotkeyDuration = 3.0f; 
        std::vector<Hotkey> eventHotkeys;
        std::vector<Hotkey> defaultHotkeys;
       
        Hotkey skipEventHotkey;
        float eventHotkeyDuration;
        Hotkey toggleInGameInteractableHotkey;
        Hotkey toggleInGameAlwaysVisibleHotkey;

        MessagesManager& messages;
        
        HotkeyManager(MessagesManager& messages);
        
        bool load();
        Hotkey queryHotkey();

        bool addHotkey(Hotkey hotkey, bool isEventHotkey);
        bool setHotkey(Hotkey hotkey, bool isEventHotkey, int index);
        bool setSkipEventHotkey(Hotkey hotkey);
        bool setToggleInGameInteractableHotkey(Hotkey hotkey);
        bool setToggleInGameAlwaysVisibleHotkey(Hotkey hotkey);
        bool removeHotkey(bool isEventHotkey, int index);
        void setEventHotkeyDuration(float duration);

    private:
        void attemptWriteToJSON();
        bool writeToJSON();
};

