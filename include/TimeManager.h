#include <Bonezegei_DS1307.h>

class TimeManager {
    public:
        TimeManager(Bonezegei_DS1307 &rtc);
        void syncTime();
        bool getUnixMs(uint64_t &timestampMs);
        const char *ntpServer = "pool.ntp.org";
        const char *tzInfo = "EST5EDT,M3.2.0,M11.1.0";
    
    private:
        bool initTime();
        Bonezegei_DS1307 _rtc;
        bool _timeReady;
};