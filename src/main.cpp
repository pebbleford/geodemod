#include <Geode/Geode.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include "ui/ModMenuPopup.hpp"

using namespace geode::prelude;

// Hook keyboard input globally to detect Tab key
class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool down, bool repeat) {
        if (down && !repeat && key == cocos2d::enumKeyCodes::KEY_Tab) {
            ModMenuPopup::toggle();
            return true;
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat);
    }
};
