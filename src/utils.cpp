
#include "utils.h"

uint64_t Utils::generateDtuSerial()
{
    uint32_t chipId = ESP.getChipId();
    uint64_t dtuId = 0;

    // Product category (char 1-4): 1 = Micro Inverter, 999 = Dummy
    dtuId |= 0x199900000000;

    // Year of production (char 5): 1 equals 2015 so hard code 8 = 2022
    dtuId |= 0x80000000;

    // Week of production (char 6-7): Range is 1-52 s hard code 1 = week 1
    dtuId |= 0x0100000;

    // Running Number (char 8-12): Derived from the ESP chip id
    for (uint8_t i = 0; i < 5; i++) {
        dtuId |= (chipId % 10) << (i * 4);
        chipId /= 10;
    }

    return dtuId;
}