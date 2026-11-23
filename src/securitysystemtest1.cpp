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
void setup();
void loop();
Bonezegei_DS1307 rtc(0x68);
// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP);

// These are 'flexible' lines that can be changed
#define TFT_CS 26
#define TFT_DC 14
#define TFT_RST 27 // RST can be set to -1 if you tie it to Arduino's reset

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

WiFiManager wifiManager = WiFiManager();
TimeManager timeManager;
ScreenManager screenManager = ScreenManager();

void setup()
{
  //  pinMode(7,OUTPUT);/
  //  digitalWrite(7,HIGH);/
  Serial.begin(9600);
  rtc.begin();
  tft.begin();
  wifiManager.init(ssid, password);
  // timeClient.begin();
  timeManager = TimeManager();
  timeManager.syncTime(rtc);
  CommunicationsScreen commsScreen = CommunicationsScreen(static_cast<ScreenID> (1), &tft);
  screenManager.addScreen(commsScreen);
}



void loop(void)
{
  tft.setRotation(3);

  StatusBar::clear(&rtc, &tft);
  StatusBar::draw(&rtc, &tft);

  /* tft.fillScreen(ST7796S_BLACK);
  if (WiFi.isConnected()) {
    tft.fillCircle(10, 32, 8, ST7796S_GREEN);
  } else {
    tft.fillCircle(10, 32, 8, convertHEXto565(0xff0000));
  }

  if (testConnection("http://whiskers.one"))
  {
    tft.fillCircle(10, 64, 8, ST7796S_GREEN);
  }
  else
  {
    tft.fillCircle(10, 64, 8, convertHEXto565(0xff0000));
  }
  if (testConnection("http://cakjhfkjhtz.wtf"))
  {
    tft.fillCircle(10, 96, 8, ST7796S_GREEN);
  }
  else
  {
    tft.fillCircle(10, 96, 8, convertHEXto565(0xff0000));
  }
  tft.setTextColor(ST7796S_WHITE);
  tft.setCursor(28, 24);
  tft.print(WiFi.isConnected() ? "WiFi connected" : "WiFi disconnected");
  tft.setCursor(28, 56);
  tft.print("http://whiskers.one/"); // wifi status
  tft.setCursor(28, 88);
  tft.print("http://cajgjhvhbtz.wtf/"); // whiskers.one status */

  delay(10000);
} /*

  //  digitalWrite(7,HIGH);
    tft.fillScreen(ST7796S_BLACK);
    tft.setRotation(0);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.println("11/08/26      | 11:18");
    tft.print("Future Home Automation Hub");
    tft.println("TODO: display site statuses");
    tft.println("http://whiskers.one - status: ???");
    tft.println("http://whiskers.wtf - status: ???");
    tft.println("http://catz.wtf     - status: ???");
    tft.println("http://whiskers.one - status: ???");
    tft.println("TODO: display light controls");
    tft.println("TODO: create rgb status indicator");
    tft.println("TODO: CREATE UI");
    tft.println("TODO: Integrate light controlling");
    tft.println("Weather for Lansing, MI: ??°c");
    tft.println("it's probably cold");
    delay(30000);

    tft.fillScreen(ST7796S_BLACK);
    tft.setRotation(2);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.println("11/08/26      | 11:19");
    tft.print("Future Home Automation Hub");
    tft.println("TODO: display site statuses");
    tft.println("http://whiskers.one - status: ???");
    tft.println("http://whiskers.wtf - status: ???");
    tft.println("http://catz.wtf     - status: ???");
    tft.println("http://whiskers.one - status: ???");
    tft.println("TODO: display light controls");
    tft.println("TODO: create rgb status indicator");
    tft.println("TODO: CREATE UI");
    tft.println("TODO: Integrate light controlling");
    tft.println("Weather for Lansing, MI: ??°c");
    tft.println("it's probably cold");
    delay(30000);


  }

  //  */
