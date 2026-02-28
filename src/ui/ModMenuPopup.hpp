#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/ScrollLayer.hpp>

using namespace geode::prelude;

struct HackItem {
    const char* name;
    const char* key;
    const char* desc;
};

class ModMenuPopup : public geode::Popup {
protected:
    ScrollLayer* m_scrollLayer = nullptr;
    CCMenu* m_tabBar = nullptr;
    CCNode* m_contentArea = nullptr;
    CCLabelBMFont* m_titleLabel = nullptr;
    CCLabelBMFont* m_activeCountLabel = nullptr;
    int m_currentTab = 0;
    std::vector<CCMenuItemToggler*> m_togglers;

    bool init() override;
    void showTab(int tab);
    CCNode* createToggleRow(const char* label, const char* key, const char* desc);
    CCNode* createSliderRow(const char* label, float value, float min, float max);
    void onTabSwitch(CCObject* sender);
    void onToggle(CCObject* sender);
    void onSpeedSlider(CCObject* sender);
    void keyDown(cocos2d::enumKeyCodes key, double p1) override;
    void updateActiveCount();
    int countActiveHacks();

public:
    static ModMenuPopup* create();
    static void toggle();
};
