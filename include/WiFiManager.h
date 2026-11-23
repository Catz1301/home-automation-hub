class WiFiManager
{
public:
    WiFiManager();
    ~WiFiManager();
    bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2);
    bool init(const char *ssid, const char *pass);
    bool reconnect();
    bool disconnect();
    static bool confirmConnection(int tryCount = 3);

private:
    static bool _initialized;
};