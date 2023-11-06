#include <Arduino.h>

#ifndef utils_h
#define utils_h

class Utils {
public:
    static uint64_t getChipId();
    static uint64_t generateDtuSerial();
};

#endif /* utils_h */