#include <Bonezegei_DS1307.h>
#include <Adafruit_ST7796S_kbv.h>
#include "utils.h"

#ifndef __STATUSBAR_H
#define __STATUSBAR_H

class StatusBar {
    public:
        static void draw(Bonezegei_DS1307 *rtc, Adafruit_ST7796S_kbv *tft);
        static void clear(Bonezegei_DS1307 *rtc, Adafruit_ST7796S_kbv *tft);
        static void updateSeconds(Bonezegei_DS1307 *rtc, Adafruit_ST7796S_kbv *tft);
};

#endif