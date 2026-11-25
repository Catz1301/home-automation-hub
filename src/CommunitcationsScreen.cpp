#include <Arduino.h>
#include "WiFiManager.h"
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S_kbv.h>
#include "Screen.h"
#include "utils.h"
#include "CommunicationsScreen.h"

CommunicationsScreen::CommunicationsScreen(ScreenID id, Adafruit_ST7796S_kbv *tft) : Screen(id, tft, true, 10000)
{
    _id = id;
    _updateIntervalMs = 10 * 1000;
    _waitForInterval = true;
    _lastUpdatedMs = INT32_MAX;
    Serial.println("[CommunicationsScreen] creating instance");
}

CommunicationsScreen::~CommunicationsScreen()
{
    Serial.println("[CommunicationsScreen] destroying instance");
}

void CommunicationsScreen::draw() {
    // tft->fillScreen(ST7796S_BLACK);
    if (WiFi.isConnected())
    {
        tft->fillCircle(10, 32, 8, ST7796S_GREEN);
    }
    else
    {
        tft->fillCircle(10, 32, 8, convertHEXto565(0xff0000));
    }

    if (testConnection("http://whiskers.one"))
    {
        tft->fillCircle(10, 64, 8, ST7796S_GREEN);
    }
    else
    {
        tft->fillCircle(10, 64, 8, convertHEXto565(0xff0000));
    }
    if (testConnection("http://cakjhfkjhtz.wtf"))
    {
        tft->fillCircle(10, 96, 8, ST7796S_GREEN);
    }
    else
    {
        tft->fillCircle(10, 96, 8, convertHEXto565(0xff0000));
    }
    tft->setTextColor(ST7796S_WHITE);
    tft->setCursor(28, 24);
    tft->print(WiFi.isConnected() ? "WiFi connected" : "WiFi disconnected");
    tft->setCursor(28, 56);
    tft->print("http://whiskers.one/"); // wifi status
    tft->setCursor(28, 88);
    tft->print("http://cajgjhvhbtz.wtf/"); // whiskers.one status
}

void CommunicationsScreen::update(uint64_t currentUnixMs)
{
    if (currentUnixMs - _lastUpdatedMs > 10000) { // 10 seconds so as to not flood my poor rpi
        this->needsRedraw = true;
    }
    _lastUpdatedMs = currentUnixMs;
}

void CommunicationsScreen::updateScreen() {
    
}