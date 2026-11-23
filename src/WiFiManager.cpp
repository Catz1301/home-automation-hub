#include <Arduino.h>
#include <WiFi.h>
#include "WiFiManager.h"
bool WiFiManager::_initialized = false;
WiFiManager::WiFiManager()
{
    WiFiManager::_initialized = false;
    // if (this->init(ssid, pass))
    //     _initialized = true;
};

WiFiManager::~WiFiManager() {
    WiFi.config(IPAddress('0.0.0.0'), IPAddress('0.0.0.0'), IPAddress('0.0.0.0'), IPAddress('0.0.0.0'), IPAddress('0.0.0.0'));
    WiFi.disconnect(false, true);
    WiFiManager::_initialized = false;
}

bool WiFiManager::init(const char *ssid, const char *pass)
{
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname("meow, meow meow");
    WiFi.begin(ssid, pass);
    // Auto reconnect is set true as default
    // To set auto connect off, use the following function
    //    WiFi.setAutoReconnect(false);

    // Will try for about 10 seconds (20x 500ms)
    int tryDelay = 500;
    int numberOfTries = 20;
    _initialized = true;
    // Wait for the WiFi event
    while (true)
    {

        switch (WiFi.status())
        {
        case WL_NO_SSID_AVAIL:
            Serial.println("[WiFi] SSID not found");
            break;
        case WL_CONNECT_FAILED:
            Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
            return false;
            break;
        case WL_CONNECTION_LOST:
            Serial.println("[WiFi] Connection was lost");
            break;
        case WL_SCAN_COMPLETED:
            Serial.println("[WiFi] Scan is completed");
            break;
        case WL_DISCONNECTED:
            Serial.println("[WiFi] WiFi is disconnected");
            break;
        case WL_CONNECTED:
            Serial.println("[WiFi] WiFi is connected!");
            Serial.print("[WiFi] IP address: ");
            Serial.println(WiFi.localIP());
            return true;
            break;
        default:
            Serial.print("[WiFi] WiFi Status: ");
            Serial.println(WiFi.status());
            break;
        }
        delay(tryDelay);

        if (numberOfTries <= 0)
        {
            Serial.print("[WiFi] Failed to connect to WiFi!");
            // Use disconnect function to force stop trying to connect
            WiFi.disconnect();
            return false;
        }
        else
        {
            numberOfTries--;
        }
    }
    return false;
}

bool WiFiManager::config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2) {
    return WiFi.config(local_ip, gateway, subnet, dns1, dns2);
}

bool WiFiManager::reconnect() {
    if (!_initialized) return false;
    return WiFi.reconnect();
}

bool WiFiManager::disconnect() {
    if (!_initialized)
        return false;
    return WiFi.disconnect();
}

bool WiFiManager::confirmConnection(int tryCount) {
    if (!_initialized)
        return false;
    int attempts = 0;
    while (attempts < tryCount) {
        if (WiFi.status() != WL_CONNECTED) {
            delay(500);
            attempts++;
            continue;
        } else {
            return true;
        }
    } // finishing the loop means no early return means failure
    return false;
}