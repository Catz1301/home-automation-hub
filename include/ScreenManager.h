#include "Screen.h"
#include <vector>
class ScreenManager {
public:
    ScreenManager();
    void updateAndDraw(uint64_t currentUnixMs);
    void addScreen(Screen &newScreen);
    void changeScreen(ScreenID newScreenId);
    private:
    Screen *_activeScreen;
    std::vector<Screen *> _screens;
    
};