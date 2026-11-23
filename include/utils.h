#include <Arduino.h>
#ifndef __UTILS_H
#define __UTILS_H

inline uint16_t convertHEXto565(uint32_t hexColor)
{
    uint r = (hexColor >> 16) & 0xFF;
    uint g = (hexColor >> 8) & 0xFF;
    uint b = hexColor >> 8 & 0xFF;

    return static_cast<uint16_t>(((r & 0xF8) << 8) |
                                 ((g & 0xFC) << 3) |
                                 (b >> 3));
}

#endif