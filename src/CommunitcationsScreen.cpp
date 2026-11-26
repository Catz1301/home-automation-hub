#include <Arduino.h>
#include "WiFiManager.h"
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S_kbv.h>
#include "Screen.h"
#include "utils.h"
#include "StatusBar.h"
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

void CommunicationsScreen::draw()
{
    tft->fillScreen(ST7796S_BLACK);
    bool wifiConnected = WiFi.isConnected(),
         whiskers_one = testConnection("http://whiskers.one/"),
         catz_wtf = testConnection("http://catz.wtf/");
    
    // Status circles
    tft->fillCircle(10, 32, 8, (wifiConnected) ? ST7796S_GREEN : convertHEXto565(0xff0000));
    tft->fillCircle(10, 64, 8, (whiskers_one) ? ST7796S_GREEN : convertHEXto565(0xff0000));
    tft->fillCircle(10, 96, 8, (catz_wtf) ? ST7796S_GREEN : convertHEXto565(0xff0000));
    tft->setTextColor(ST7796S_WHITE);
    tft->setCursor(28, 24);
    tft->print(WiFi.isConnected() ? "WiFi connected" : "WiFi disconnected");
    tft->setCursor(28, 56);
    tft->print("http://whiskers.one/"); // wifi status
    tft->setCursor(28, 88);
    tft->print("http://catz.wtf/"); // whiskers.one status

    needsRedraw = false; 
}

void CommunicationsScreen::update(uint64_t currentUnixMs)
{
    /* Serial.print("[CommunicationScreen] currentUnixMs = ");
    Serial.println((currentUnixMs));
    Serial.print("[CommunicationScreen] _lastUpdatedMs = ");
    Serial.println((currentUnixMs));
    Serial.print("[CommunicationScreen] currentUnixMs - _lastUpdatedMs = ");
    Serial.println((currentUnixMs - _lastUpdatedMs)); */
    if (_lastUpdatedMs == INT32_MAX) { // not drawn yet
        // get global status bar height
        needsRedraw = true; // because we haven't drawn yet
        // fill rect at 0, 0+globalStatusBarHeight, 480, 320
        tft->fillRect(0, __STATUSBAR_HEIGHT, 480, 320-__STATUSBAR_HEIGHT, convertHEXto565(0x38333B));
        _lastUpdatedMs = currentUnixMs;
    }
    if (currentUnixMs - _lastUpdatedMs >= 10000)
    { // 10 seconds so as to not flood my poor rrrrrrrrrrrrrrrrrrrrrrdddddpi
        
        this->needsUpdate = true;
        _lastUpdatedMs = currentUnixMs;
    }
}

void CommunicationsScreen::updateScreen(uint8_t reasonFlag = 0)
{
    bool wifiConnected = WiFi.isConnected(), whiskers_one = testConnection("http://whiskers.one/"), catz_wtf = testConnection("http://catz.wtf/");
    tft->fillCircle(10, 32, 8, (wifiConnected) ? ST7796S_GREEN : convertHEXto565(0xff0000));
    tft->fillCircle(10, 64, 8, (whiskers_one) ? ST7796S_GREEN : convertHEXto565(0xff0000));
    tft->fillCircle(10, 96, 8, (catz_wtf) ? ST7796S_GREEN : convertHEXto565(0xff0000));
    tft->setTextColor(ST7796S_WHITE);
    tft->setCursor(28, 24);
    tft->print(WiFi.isConnected() ? "WiFi connected" : "WiFi disconnected");
    tft->setCursor(28, 56);
    tft->print("http://whiskers.one/"); // wifi status
    tft->setCursor(28, 88);
    tft->print("http://catz.wtf/"); // whiskers.one status
    needsUpdate = false; // at end of updateScreen method. we're done and dope.
}