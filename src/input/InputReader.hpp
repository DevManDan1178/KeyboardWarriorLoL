#pragma once

#include <uiohook.h>
#include <functional>
#include <unordered_map>
#include <vector>
#include "managers/HotkeyManager.hpp" 

using HotkeyCallback = std::function<void()>;

struct HotkeyBinding {
    Hotkey hotkey;
    HotkeyCallback callback;
};


class InputReader {
public:
    static bool start();
    static void stop();

    static void onHotkey(const Hotkey& hotkey, HotkeyCallback callback);
    static void onHotkeyRelease(const Hotkey& hotkey, HotkeyCallback callback);
    static void clearHotkeys();

    static uint8_t currentModifiers();

    static bool isKeyDown(int keycode);
    static void dispatch(uiohook_event* event);
    
private:
    
    static bool matchesModifiers(uint8_t required);

    

};