#pragma once

#include "../external/json.hpp"
#include "managers/MessagesManager.hpp"
#include "managers/HotkeyManager.hpp"
#include <string>
#include "unordered_map"
#include <variant>
#include "lol/LoLTypes.hpp"
#include "lol/LoLChatSender.hpp"
#include <functional>
#include <queue>
#include <tuple>
#include <chrono>

using json = nlohmann::json;


//Event Categories
class LoLEventHandler {
    public:
        std::queue<std::tuple<std::string, std::string>> eventQueue; //{Category, eventName}
        MessagesManager &messages;
        HotkeyManager &hotkeyManager;
        LoLPlayersInfo playersInfo;
        LoLChatSender &chatSender;
        std::function<void(std::string, std::string)> &displayEventChange;

        std::chrono::steady_clock::time_point currentEventStartTime;


        LoLEventHandler(MessagesManager &_messages, HotkeyManager &_hotkeyManager, LoLChatSender &_chatSender, std::function<void(std::string, std::string)> &_displayEventChange);
        void processLoLEvent(json LoLEvent);
        
        void closeCurrentEvent();
        void process();

        void processHotkeyPressed(int hotkeyIndex, bool isEvent);
        float getHotkeyExpirationProgress();
        void reset();

        std::tuple<std::string, std::string> getCurrentEvent();
        std::tuple<std::string, std::string> getNextEvent();
    private:   
        void updateCurrentEventStartTime();
        void queueLoLEvent(std::string eventCategory, std::string eventName, bool forceAsCurrent = false);
        void printPlayersInfo();
        float getEventHotkeyDuration();
};