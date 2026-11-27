#include <Arduino.h>
#include "TouchManager.h"
#include "Navigator.h"

Navigator::Navigator()
{
    lastShownMs = 0;
}

void Navigator::init(Adafruit_GFX *gfx)
{
    _previousScreenBtn.initButtonUL(gfx, 10, 285, 25, 25, ST7796S_GREENYELLOW, ST7796S_DARKGREY, ST7796S_WHITE, "<", 2, 2);
    _nextScreenBtn.initButtonUL(gfx, 445, 285, 25, 25, ST7796S_GREENYELLOW, ST7796S_DARKGREY, ST7796S_WHITE, ">", 2, 2);
}

void Navigator::drawNavButtons(Adafruit_ST7796S_kbv *tft)
{
    if (!_navInactive)
    {
        // lastShownMs = millis();
        if (millis() - lastShownMs >= 5000)
        {
            _navInactive = true;
        }
        else
        {
            _previousScreenBtn.drawButton();
            _nextScreenBtn.drawButton();
        }
    }
}

void Navigator::update(TouchManager *tm)
{
    if (tm != nullptr)
    {
        if (tm->touched())
        {
            TS_Point tp = tm->getTouchedPoint();
            if (tp.y > 285)
            {
                _navInactive = false;
                lastShownMs = millis();
            }

            if (_previousScreenBtn.contains(tp.x, tp.y))
            { // previousButton
                _previousScreenBtn.press(true);
            }
            if (_previousScreenBtn.contains(tp.x, tp.y))
            { // previousButton
                _previousScreenBtn.press(true);
            }
        }
        else
        {
            _previousScreenBtn.press(false);
            _nextScreenBtn.press(false);
        }
    }
}