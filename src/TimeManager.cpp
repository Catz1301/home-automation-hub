#include <Arduino.h>
#include "WiFiManager.h"
#include <time.h>
#include "TimeManager.h"
#include <Bonezegei_DS1307.h>

TimeManager::TimeManager(Bonezegei_DS1307 &rtc) : _rtc(rtc), _timeReady(false)
{
    _timeReady = initTime();
};

bool TimeManager::initTime()
{
    // try 3 times
    Serial.println("[Time] Initializing NTP");
    int tryCount = 3;
    if (!WiFiManager::confirmConnection())
    {
        Serial.println("[Time] Not connected to internet! Retrying...");
        return false;
    }
    configTzTime(this->tzInfo, this->ntpServer);

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("[TimeManager] Initialization failed; could not get local time");
        _timeReady = false;
    }
    else
    {
        Serial.println("[TimeManager] Initialization progress. Retrieved local time");
        int hour = timeinfo.tm_hour;
        int minute = timeinfo.tm_min;
        int second = timeinfo.tm_sec;

        int month = timeinfo.tm_mon + 1;
        int day = timeinfo.tm_mday;
        int year = timeinfo.tm_year % 100;
        char time[32], date[32];
        snprintf(time, sizeof(time), "%02d:%02d:%02d", hour, minute, second);
        snprintf(date, sizeof(date), "%d/%d/%d", month, day, year);
        _rtc.setTime(time);
        _rtc.setDate(date);

        _timeReady = true;
    }
    Serial.println("[TimeManager] initialization complete.");
    Serial.print("[TimeManager] Time is ");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    return _timeReady;
};

void TimeManager::syncTime()
{

    if (!WiFiManager::confirmConnection(3) || !_timeReady)
        return;

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
        return;

    int hour = timeinfo.tm_hour;
    int minute = timeinfo.tm_min;
    int second = timeinfo.tm_sec;

    int month = timeinfo.tm_mon + 1;
    int day = timeinfo.tm_mday;
    int year = timeinfo.tm_year % 100;
    char time[32], date[32];
    snprintf(time, sizeof(time), "%02d:%02d:%02d", hour, minute, second);
    snprintf(date, sizeof(date), "%d/%d/%d", month, day, year);
    _rtc.setTime(time);
    _rtc.setDate(date);

    

    Serial.println(_rtc.getDate());
    Serial.println(_rtc.getYear());
    // rtc.setDay(timeinfo.tm_wday);
    
}

bool TimeManager::getUnixMs(uint64_t &timestampMs)
{
    if (!_rtc.getTime())
        return false;

    struct tm timeinfo = {};
    const uint8_t format = _rtc.getFormat();
    int hour = _rtc.getHour();

    if (format == 12)
    {
        const bool isPm = _rtc.getAMPM() == 0;
        if (hour < 1 || hour > 12)
            return false;
        hour = hour % 12 + (isPm ? 12 : 0);
    }

    const int month = _rtc.getMonth();
    const int day = _rtc.getDate();
    const int year = _rtc.getYear();
    const int minute = _rtc.getMinute();
    const int second = _rtc.getSeconds();
    if (month < 1 || month > 12 || day < 1 || day > 31 ||
        hour > 23 || minute > 59 || second > 59)
        return false;

    timeinfo.tm_year = 100 + year;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;
    timeinfo.tm_isdst = -1;

    setenv("TZ", tzInfo, 1);
    tzset();
    const time_t unixSeconds = mktime(&timeinfo);
    if (unixSeconds < 0 || timeinfo.tm_year != 100 + year ||
        timeinfo.tm_mon != month - 1 || timeinfo.tm_mday != day ||
        timeinfo.tm_hour != hour || timeinfo.tm_min != minute ||
        timeinfo.tm_sec != second)
        return false;

    timestampMs = static_cast<uint64_t>(unixSeconds) * 1000;
    return true;
}