#ifndef __SCREEN_H
#define __SCREEN_H
typedef int ScreenID;
#include <Arduino.h>
#include <Adafruit_ST7796S_kbv.h>

class Screen {
    public:
    Screen(int id, Adafruit_ST7796S_kbv *tftScreen) : _id(id), tft(tftScreen) {}
    ~Screen() {
        // free(tft);
    }
    virtual void draw() = 0;
    virtual void update(uint64_t currentUnixMs) = 0; // for main loop
    virtual void updateScreen() = 0; // if this screen needs an update
    const ScreenID getId() {
        return _id;
    }
    bool needsUpdate = false; // if this screen needs an update, it will be done in updateScreen()
    bool needsRedraw = false; // if this screen needs a complete redraw, do it in draw()
    protected:
    bool _waitForInterval;
    int _updateIntervalMs;
    uint64_t  _lastUpdatedMs;
    ScreenID _id;
    Adafruit_ST7796S_kbv *tft;
};



#endif