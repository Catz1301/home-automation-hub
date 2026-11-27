#include "Screen.h"
#include <vector>
class ScreenManager {
public:
    ScreenManager();
    ~ScreenManager();
    void updateAndDraw(uint64_t currentUnixMs);
    void addScreen(Screen *newScreen);
    void changeScreen(ScreenID newScreenId);
    Screen* getActiveScreen();
    bool requestHideStatusBar = false;
    private:
    Screen *_activeScreen;
    std::vector<Screen *> _screens;
    
};