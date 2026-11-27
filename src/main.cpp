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
#include "TouchManager.h"
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
TouchManager touchManager = TouchManager(TC_CS, TC_IRQ);
SPIClass tcSPI = SPIClass();
WiFiManager wifiManager = WiFiManager();
TimeManager timeManager = TimeManager(rtc);
ScreenManager screenManager = ScreenManager();
Preferences preferences;

// Screens
CommunicationsScreen commsScreen = CommunicationsScreen(static_cast<ScreenID>(1), &tft);
ulong lastLoopus = 0;


void setup()
{
  //  pinMode(7,OUTPUT);/
  //  digitalWrite(7,HIGH);/
  Serial.begin(9600);
  rtc.begin();
  tft.begin();
  tft.setRotation(3);
  // tcSPI.begin(TC_SCK, TC_MISO, TC_MOSI, TC_IRQ);
  touchManager.init(TC_CS, TC_MISO, TC_MOSI, TC_SCK);
  touchManager.tc.setRotation(3);
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
  touchManager.tc.setRotation(3);
  if (Serial.available())
  {
    char command = Serial.read();
    if (command == 'c' || command == 'C')
    {
      touchManager.runTouchCalibration(screenManager.getActiveScreen(), &tft);
    }
  }
  if (touchManager.tc.touched()) {
    Serial.println("[TouchManager] touch detected");
    TS_Point mappedTp = touchManager.getTouchedPoint();
    tft.fillCircle(mappedTp.x, mappedTp.y, 4, ST7796S_CYAN);
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