// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <Arduino.h>
#include <cstdint>


typedef enum {
    CMD_OK,
    CMD_NOK,
    CMD_PENDING
} LastCommandSuccess;

class Parser {
public:
    Parser();
    uint32_t getLastUpdate();
    void setLastUpdate(uint32_t lastUpdate);

    void beginAppendFragment();
    void endAppendFragment();
    
private:
    uint32_t _lastUpdate = 0;
};