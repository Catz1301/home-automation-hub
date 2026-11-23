#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ST7796S_kbv.h"
#include "StatusBar.h"
void StatusBar::updateSeconds(Bonezegei_DS1307 *rtc, Adafruit_ST7796S_kbv *tft)
{
    if (rtc->getTime()) {
        tft->setCursor(74, 2);
        tft->setTextColor(ST7796S_BLACK, convertHEXto565(0xD9AE94));
        tft->print(rtc->getSeconds());
    }
}
void StatusBar::clear(Bonezegei_DS1307 *rtc, Adafruit_ST7796S_kbv *tft) {
    tft->fillRoundRect(0, 0, 480, 18, 0, convertHEXto565(0xD9AE94));
}
void StatusBar::draw(Bonezegei_DS1307 *rtc, Adafruit_ST7796S_kbv *tft) {
    tft->fillRoundRect(0, 0, 480, 18, 0, convertHEXto565(0xD9AE94));
    if (rtc->getTime())
    {
        // Serial.printf only does not work on atmega
        // Serial.printf("Time %02d:%02d:%02d ", rtc.getHour(), rtc.getMinute(), rtc.getSeconds());
        char time_data[32];
        sprintf(time_data, "%02d:%02d:%02d", rtc->getHour(), rtc->getMinute(), rtc->getSeconds());
        char date_data[32];
        sprintf(date_data, "%02d/%02d/%02d", rtc->getMonth(), rtc->getDate(), rtc->getYear());
        tft->setCursor(2, 2);
        tft->setTextColor(ST7796S_BLACK, convertHEXto565(0xD9AE94));
        tft->setTextSize(2);
        tft->print(time_data);
        tft->setCursor(384, 2);
        Serial.printf("Date %02d-%02d-%d \n", rtc->getMonth(), rtc->getDate(), 26);
        Serial.printf(date_data);
        tft->print(date_data);
    }
}