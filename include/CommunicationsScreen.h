#include <Arduino.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S_kbv.h>

#include "Screen.h"

class CommunicationsScreen : Screen {
    public:
    CommunicationsScreen(ScreenID id, Adafruit_ST7796S_kbv *tft);
    void draw(); // if screen needs redraw, go here;
    void update(uint64_t currentUnixMs); // from main loop. if redraw isn't needed, draw returns immediately
    void updateScreen(); //to update certain parts (buttons, circles, etc)

    bool testConnection(char *str)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;

            // Specify the target URL
            http.begin(str);

            // Send the HTTP GET request
            int httpResponseCode = http.GET();

            if (httpResponseCode == 200)
            {
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
                http.end();
                return true;
            }
            else
            {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
                http.end();
                return false;
            }
        }
        return false; // implies server offline, when we're offline
    }

    private:
};