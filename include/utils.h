// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Modified by Fa-b, 07-11-2023
 */

#include <Arduino.h>

#ifndef utils_h
#define utils_h

class Utils {
public:
    static uint64_t getChipId();
    static uint64_t generateDtuSerial();
};

#endif /* utils_h */