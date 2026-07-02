#pragma once

#include "managers/HotkeyManager.hpp"
#include "managers/MessagesManager.hpp"

namespace MessagesUI {
    extern int categoryToggleStates;
    void messagesMenu(MessagesManager& messages, HotkeyManager &hotkeyManager);
}