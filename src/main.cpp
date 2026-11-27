#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiScan.h>
#include <WiFiMulti.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiGeneric.h>

/***************************************************
  This is our library for the Adafruit HX8357D Breakout
  ----> http://www.adafruit.com/products/2050

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ST7796S_kbv.h"
#include "utils.h"
#include <HTTPClient.h>
#include "StatusBar.h"
#include "WiFiManager.h"
#include "TimeManager.h"
#include "Screen.h"
#include "ScreenManager.h"
#include "CommunicationsScreen.h"

#include <Bonezegei_DS1307.h>
#include <XPT2046_Touchscreen.h>
#include <Preferences.h>
void setup();
void loop();
Bonezegei_DS1307 rtc(0x68);
// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP);

// These are 'flexible' lines that can be changed
#define TFT_CS 26
#define TFT_DC 25
#define TFT_RST 27 // RST can be set to -1 if you tie it to Arduino's reset

#define TC_SCK 13
#define TC_MOSI 5
#define TC_MOSI 5
#define TC_MISO 12
#define TC_CS 33
#define TC_IRQ 32
const char *ssid = "ARRIS-959D";
const char *password = "4Ym6jHhd4y7y";
// const char *ntpServer = "pool.ntp.org";

// const long gmtOffset_sec = -18000;
// const char *tzInfo = "EST5EDT,M3.2.0,M11.1.0";

// volatile bool timeReady = false;
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ST7796S_kbv tft = Adafruit_ST7796S_kbv(TFT_CS, TFT_DC, TFT_RST);
// SoftSPI - note that on some processors this might be *faster* than hardware SPI!
// Adafruit_ST7796S_kbv tft = Adafruit_ST7796S_kbv(TFT_CS, TFT_DC, MOSI, SCK, TFT_RST, MISO);
XPT2046_Touchscreen tc = XPT2046_Touchscreen(TC_CS, TC_IRQ);
SPIClass tcSPI = SPIClass();
WiFiManager wifiManager = WiFiManager();
TimeManager timeManager = TimeManager(rtc);
ScreenManager screenManager = ScreenManager();
Preferences preferences;

// Screens
CommunicationsScreen commsScreen = CommunicationsScreen(static_cast<ScreenID>(1), &tft);
ulong lastLoopus = 0;

struct TouchCalibration
{
  int16_t leftX;
  int16_t rightX;
  int16_t topY;
  int16_t bottomY;
};

constexpr int16_t SCREEN_WIDTH = 480;
constexpr int16_t SCREEN_HEIGHT = 320;
constexpr int16_t CALIBRATION_LEFT = 20;
constexpr int16_t CALIBRATION_RIGHT = SCREEN_WIDTH - 21;
constexpr int16_t CALIBRATION_TOP = 20;
constexpr int16_t CALIBRATION_BOTTOM = SCREEN_HEIGHT - 21;

TouchCalibration touchCalibration = {200, 3900, 200, 3900};

void waitForTouchRelease()
{
  while (tc.touched())
  {
    delay(10);
  }
}

TS_Point sampleTouchPoint(const char *label)
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

void runTouchCalibration()
{
  tft.fillScreen(ST7796S_BLACK);
  tft.setTextColor(ST7796S_WHITE, ST7796S_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.print("Touch calibration");
  tft.setCursor(20, 125);
  tft.print("Press each target");
  delay(500);
  waitForTouchRelease();

  tft.fillScreen(ST7796S_BLACK);
  tft.drawCircle(CALIBRATION_LEFT, CALIBRATION_TOP, 12, ST7796S_RED);
  tft.fillCircle(CALIBRATION_LEFT, CALIBRATION_TOP, 3, ST7796S_RED);
  TS_Point topLeft = sampleTouchPoint("Top-left");

  tft.fillScreen(ST7796S_BLACK);
  tft.drawCircle(CALIBRATION_RIGHT, CALIBRATION_BOTTOM, 12, ST7796S_RED);
  tft.fillCircle(CALIBRATION_RIGHT, CALIBRATION_BOTTOM, 3, ST7796S_RED);
  TS_Point bottomRight = sampleTouchPoint("Bottom-right");

  if (topLeft.z == 0 || bottomRight.z == 0 ||
      topLeft.x == bottomRight.x || topLeft.y == bottomRight.y)
  {
    Serial.println("Calibration failed. Try again.");
    return;
  }

  touchCalibration.leftX = topLeft.x;
  touchCalibration.rightX = bottomRight.x;
  touchCalibration.topY = topLeft.y;
  touchCalibration.bottomY = bottomRight.y;

  preferences.begin("touch", false);
  preferences.putShort("leftX", touchCalibration.leftX);
  preferences.putShort("rightX", touchCalibration.rightX);
  preferences.putShort("topY", touchCalibration.topY);
  preferences.putShort("bottomY", touchCalibration.bottomY);
  preferences.end();

  Serial.printf(
      "Saved calibration: leftX=%d rightX=%d topY=%d bottomY=%d\n",
      touchCalibration.leftX,
      touchCalibration.rightX,
      touchCalibration.topY,
      touchCalibration.bottomY);
  tft.fillScreen(ST7796S_GREEN);
  delay(500);
  commsScreen.draw();
  commsScreen.needsRedraw = false;
}

int32_t mapTouchCoordinate(int32_t value, int32_t rawA, int32_t rawB,
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

void loadTouchCalibration()
{
  preferences.begin("touch", true);
  touchCalibration.leftX = preferences.getShort("leftX", touchCalibration.leftX);
  touchCalibration.rightX = preferences.getShort("rightX", touchCalibration.rightX);
  touchCalibration.topY = preferences.getShort("topY", touchCalibration.topY);
  touchCalibration.bottomY = preferences.getShort("bottomY", touchCalibration.bottomY);
  preferences.end();
}

void setup()
{
  //  pinMode(7,OUTPUT);/
  //  digitalWrite(7,HIGH);/
  Serial.begin(9600);
  rtc.begin();
  tft.begin();
  tft.setRotation(3);
  tcSPI.begin(TC_SCK, TC_MISO, TC_MOSI, TC_IRQ);
  tc.begin(tcSPI);
  tc.setRotation(3);
  loadTouchCalibration();
  wifiManager.init(ssid, password);
  // timeClient.begin();

  screenManager.addScreen(&commsScreen);
  screenManager.changeScreen(static_cast<ScreenID>(1));
  Serial.println("Send 'c' over serial to start touch calibration.");
}

void loop(void)
{
  ulong startus = micros();
  tft.setRotation(3);
  tc.setRotation(3);
  if (Serial.available())
  {
    char command = Serial.read();
    if (command == 'c' || command == 'C')
    {
      runTouchCalibration();
    }
  }
  if (tc.touched()) {
    Serial.println("[TouchManager] touch detected");
    TS_Point touchPoint = tc.getPoint();
    int32_t screenX = mapTouchCoordinate(
        touchPoint.x, touchCalibration.leftX, touchCalibration.rightX,
        CALIBRATION_LEFT, CALIBRATION_RIGHT, SCREEN_WIDTH);
    int32_t screenY = mapTouchCoordinate(
        touchPoint.y, touchCalibration.topY, touchCalibration.bottomY,
        CALIBRATION_TOP, CALIBRATION_BOTTOM, SCREEN_HEIGHT);
    tft.fillCircle(screenX, screenY, 4, ST7796S_CYAN);
  }
  uint64_t currentTimeMs = 0;
  if ((startus - lastLoopus >= 99000) || (lastLoopus == 0 || lastLoopus > startus))
  {
    if (!screenManager.requestHideStatusBar)
    {
      StatusBar::clear(&rtc, &tft);
      StatusBar::draw(&rtc, &tft);
    }
    lastLoopus = micros();
  }
  timeManager.getUnixMs(currentTimeMs);
  screenManager.updateAndDraw(currentTimeMs);
  // delay ();
}