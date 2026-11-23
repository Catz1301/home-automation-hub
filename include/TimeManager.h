#include <Bonezegei_DS1307.h>

class TimeManager {
    public:
    TimeManager();
    void syncTime(Bonezegei_DS1307 &rtc);
    bool getUnixMs(Bonezegei_DS1307 &rtc, uint64_t &timestampMs);
    const char *ntpServer = "pool.ntp.org";
    const char *tzInfo = "EST5EDT,M3.2.0,M11.1.0";
    
    private:
    bool initTime();
    bool _timeReady;
};