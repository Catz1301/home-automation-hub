#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <Adafruit_ST7796S_kbv.h>
#include "TouchManager.h"
// compliance
class Navigator
{
public:
    Navigator();
    void init(Adafruit_GFX *gfx);
    void drawNavButtons(Adafruit_ST7796S_kbv *tft);
    void update(TouchManager *tm);
    uint64_t lastShownMs;

private:
    Adafruit_GFX_Button _previousScreenBtn;
    Adafruit_GFX_Button _nextScreenBtn;
    bool _navInactive = true;

};

#endif