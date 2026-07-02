#pragma once
#include <SDL.h>
#include "input/Hotkey.hpp"
#include <string>

namespace HotkeyConverter {
    int SDLToUiohook(SDL_Scancode sc);
    std::string hotkeyToString(const Hotkey& hotkey);
}