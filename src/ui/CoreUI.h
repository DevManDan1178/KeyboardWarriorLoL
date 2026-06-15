#pragma once

#include "LoLEventHandler.h"
#include "MessagesManager.h"
#include "HotkeyManager.h"
#include <SDL_syswm.h>

namespace CoreUI {
    int getEventOverlayUIFrameHeight(size_t defaultMessageListCount, size_t eventMessageListCount, bool hasNextEvent);
    void eventsOverlayUIFrame(SDL_Window* window, HWND hwnd, LoLEventHandler& lolEventHandler, MessagesManager& messages, HotkeyManager& hotkeyManager, bool interactable, bool& visibleAlways);
    void mainUIFrame(SDL_Window* window, HWND hwnd, MessagesManager& messages, HotkeyManager& hotkeyManager);
}