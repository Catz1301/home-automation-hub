#include <Arduino.h>
#include <Preferences.h>
#include <XPT2046_Touchscreen.h>
#include "Screen.h"

class TouchManager {
    public:
        TouchManager(uint8_t csPin, uint8_t irqPin = 255);
        void init(uint8_t csPin, uint8_t misoPin, uint8_t mosiPin, uint8_t sckPin);
        void waitForTouchRelease();
        TS_Point sampleTouchPoint(const char *label);
        void runTouchCalibration(Screen *activeScreen, Adafruit_ST7796S_kbv *tft);
        int mapTouchCoordinate(int value, int rawA, int rawB, int screenA, int screenB, int screenSize);
        void loadTouchCalibration();
        TS_Point getTouchedPoint();

        struct TouchCalibration
        {
            int16_t leftX;
            int16_t rightX;
            int16_t topY;
            int16_t bottomY;
        };
    public:
    XPT2046_Touchscreen tc;
    
    private:
        const int16_t _SCREEN_WIDTH = 480;
        const int16_t _SCREEN_HEIGHT = 320;
        const int16_t _CALIBRATION_LEFT = 20;
        const int16_t _CALIBRATION_RIGHT = _SCREEN_WIDTH - 21;
        const int16_t _CALIBRATION_TOP = 20;
        const int16_t _CALIBRATION_BOTTOM = _SCREEN_HEIGHT - 21;
        TouchCalibration _touchCalibration;
        Preferences _preferences;
        SPIClass _tcSPI;
};