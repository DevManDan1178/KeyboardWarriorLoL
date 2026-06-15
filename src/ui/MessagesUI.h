#pragma once

#include "HotkeyManager.h"
#include "MessagesManager.h"

namespace MessagesUI {
    extern int categoryToggleStates;
    void messagesMenu(MessagesManager& messages, HotkeyManager &hotkeyManager);
}