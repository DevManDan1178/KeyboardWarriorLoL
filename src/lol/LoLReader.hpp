#pragma once
#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include <tuple>
#include <queue>
#include "../external/json.hpp"
#include "lol/LoLEventHandler.hpp"
#include "LoLTypes.h"

const std::string ALL_GAME_DATA_QUERY_URL = "https://127.0.0.1:2999/liveclientdata/allgamedata";
const std::string PLAYER_NAME_QUERY_URL = "https://127.0.0.1:2999/liveclientdata/activeplayername";
const std::string GAME_EVENTS_QUERY_URL_PREFIX = "https://127.0.0.1:2999/liveclientdata/eventdata?eventID=";
const std::string PLAYER_LIST_QUERY_URL = "https://127.0.0.1:2999/liveclientdata/playerlist";

const int IDLE_TIME_BETWEEN_CHECKS = 2000; 
const int LOADING_TIME_BETWEEN_CHECKS = 500;
const int TIME_BETWEEN_EVENT_LOOP = 100; 

class LoLReader {
    public: 
        LoLEventHandler &lolEventHandler;
        
        void initializeLoop();
        void closeLoop();
        void process();
        
        LoLReader(LoLEventHandler& _lolEventHandler);
        bool isIdle = true; 
        bool isInGame = false;
    private:
        std::atomic<bool> running;  
        std::thread workerThread;

        void stopCoreLoop();
        void liveClientEventLoop();
        void coreLoop();
        bool queryForGame();
        bool isLoadingOrInGame();

        std::tuple<bool, LoLPlayersInfo> getPlayersInfo();

};