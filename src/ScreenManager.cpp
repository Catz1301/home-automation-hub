#include "ScreenManager.h"
#include "Screen.h"

ScreenManager::ScreenManager() {

}

ScreenManager::~ScreenManager() {
    for (int i = 0; i < _screens.size(); ++i) {
        delete _screens.at(i);
    }
    delete _activeScreen;
    _screens.clear(); 
}

void ScreenManager::updateAndDraw(uint64_t currentUnixMs) {
    _activeScreen->update(currentUnixMs);
    if (_activeScreen->needsUpdate) {
        _activeScreen->updateScreen();
    } if (_activeScreen->needsRedraw) {
        _activeScreen->draw();
    }
}

void ScreenManager::addScreen(Screen &newScreen) {
    _screens.push_back(&newScreen);
}

void ScreenManager::changeScreen(ScreenID newScreenId) {
    for (int i = 0; i < _screens.size(); i++) {
        if (newScreenId == _screens.at(i)->getId()) {
            _activeScreen = _screens.at(i);
        }
    } 
}