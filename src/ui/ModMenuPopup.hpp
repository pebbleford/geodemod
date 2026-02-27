#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

class ModMenuPopup : public geode::Popup<> {
protected:
    CCMenu* m_toggleMenu = nullptr;
    int m_currentTab = 0;

    bool init() override;
    void showTab(int tab);
    void addToggle(const char* label, const char* settingKey);
    void addSpeedSlider();
    void onTabSwitch(CCObject* sender);
    void onToggle(CCObject* sender);
    void onSpeedSlider(CCObject* sender);
    void keyDown(cocos2d::enumKeyCodes key) override;

public:
    static ModMenuPopup* create();
    static void toggle();
};
