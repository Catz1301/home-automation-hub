#include <Arduino.h>
#include <XPT2046_Touchscreen.h>
#include "TouchManager.h"

TouchManager::TouchManager(uint8_t csPin, uint8_t irqPin) : tc(csPin, irqPin)
{
}

void TouchManager::init(uint8_t csPin, uint8_t misoPin, uint8_t mosiPin, uint8_t sckPin)
{
    _touchCalibration = {
        200,
        3900,
        200,
        3900
    };
    loadTouchCalibration();
    _tcSPI.begin(sckPin, misoPin, mosiPin, csPin);
    tc.begin(_tcSPI);
    // return true;
}

void TouchManager::waitForTouchRelease()
{
    while (tc.touched())
    {
        delay(10);
    }
}

TS_Point TouchManager::sampleTouchPoint(const char *label)
{
    Serial.print(label);
    Serial.println(": press and hold the target");

    while (!tc.touched())
    {
        delay(10);
    }

    int32_t x = 0;
    int32_t y = 0;
    int sampleCount = 0;
    while (tc.touched())
    {
        TS_Point point = tc.getPoint();
        if (point.z > 0)
        {
            x += point.x;
            y += point.y;
            sampleCount++;
        }
        delay(10);
    }

    TS_Point result;
    result.x = sampleCount > 0 ? static_cast<int16_t>(x / sampleCount) : 0;
    result.y = sampleCount > 0 ? static_cast<int16_t>(y / sampleCount) : 0;
    result.z = sampleCount > 0 ? 1 : 0;
    Serial.printf("  raw X=%d Y=%d samples=%d\n", result.x, result.y, sampleCount);
    return result;
}

void TouchManager::runTouchCalibration(Screen *activeScreen, Adafruit_ST7796S_kbv *tft)
{
    tft->fillScreen(ST7796S_BLACK);
    tft->setTextColor(ST7796S_WHITE, ST7796S_BLACK);
    tft->setTextSize(2);
    tft->setCursor(20, 100);
    tft->print("Touch calibration");
    tft->setCursor(20, 125);
    tft->print("Press each target");
    delay(500);
    waitForTouchRelease();

    tft->fillScreen(ST7796S_BLACK);
    tft->drawCircle(_CALIBRATION_LEFT, _CALIBRATION_TOP, 12, ST7796S_RED);
    tft->fillCircle(_CALIBRATION_LEFT, _CALIBRATION_TOP, 3, ST7796S_RED);
    TS_Point topLeft = sampleTouchPoint("Top-left");

    tft->fillScreen(ST7796S_BLACK);
    tft->drawCircle(_CALIBRATION_RIGHT, _CALIBRATION_BOTTOM, 12, ST7796S_RED);
    tft->fillCircle(_CALIBRATION_RIGHT, _CALIBRATION_BOTTOM, 3, ST7796S_RED);
    TS_Point bottomRight = sampleTouchPoint("Bottom-right");

    if (topLeft.z == 0 || bottomRight.z == 0 ||
        topLeft.x == bottomRight.x || topLeft.y == bottomRight.y)
    {
        Serial.println("Calibration failed. Try again.");
        return;
    }

    _touchCalibration.leftX = topLeft.x;
    _touchCalibration.rightX = bottomRight.x;
    _touchCalibration.topY = topLeft.y;
    _touchCalibration.bottomY = bottomRight.y;

    _preferences.begin("touch", false);
    _preferences.putShort("leftX", _touchCalibration.leftX);
    _preferences.putShort("rightX", _touchCalibration.rightX);
    _preferences.putShort("topY", _touchCalibration.topY);
    _preferences.putShort("bottomY", _touchCalibration.bottomY);
    _preferences.end();

    Serial.printf(
        "Saved calibration: leftX=%d rightX=%d topY=%d bottomY=%d\n",
        _touchCalibration.leftX,
        _touchCalibration.rightX,
        _touchCalibration.topY,
        _touchCalibration.bottomY);
    tft->fillScreen(ST7796S_GREEN);
    delay(500);
    activeScreen->draw();
    activeScreen->needsRedraw = false;
}

int TouchManager::mapTouchCoordinate(int32_t value, int32_t rawA, int32_t rawB,
                                     int32_t screenA, int32_t screenB,
                                     int32_t screenSize)
{
    if (rawA == rawB)
    {
        return screenA;
    }

    int32_t mapped = screenA +
                     ((value - rawA) * (screenB - screenA)) / (rawB - rawA);
    if (mapped < 0)
    {
        return 0;
    }
    if (mapped >= screenSize)
    {
        return screenSize - 1;
    }
    return mapped;
}

void TouchManager::loadTouchCalibration()
{
    _preferences.begin("touch", true);
    _touchCalibration.leftX = _preferences.getShort("leftX", _touchCalibration.leftX);
    _touchCalibration.rightX = _preferences.getShort("rightX", _touchCalibration.rightX);
    _touchCalibration.topY = _preferences.getShort("topY", _touchCalibration.topY);
    _touchCalibration.bottomY = _preferences.getShort("bottomY", _touchCalibration.bottomY);
    _preferences.end();
}

TS_Point TouchManager::getTouchedPoint()
{
    TS_Point touchPoint = tc.getPoint();
    int screenX = mapTouchCoordinate(
        touchPoint.x, _touchCalibration.leftX, _touchCalibration.rightX,
        _CALIBRATION_LEFT, _CALIBRATION_RIGHT, _SCREEN_WIDTH);
    int screenY = mapTouchCoordinate(
        touchPoint.y, _touchCalibration.topY, _touchCalibration.bottomY,
        _CALIBRATION_TOP, _CALIBRATION_BOTTOM, _SCREEN_HEIGHT);

    return TS_Point{screenX, screenY, touchPoint.z};
}