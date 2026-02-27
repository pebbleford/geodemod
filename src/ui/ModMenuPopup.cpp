#include "ModMenuPopup.hpp"

bool ModMenuPopup::init() {
    if (!Popup::init(380.f, 280.f))
        return false;

    this->setTitle("GD Mod Menu");

    // Tab buttons at the top
    auto tabMenu = CCMenu::create();
    tabMenu->setLayout(RowLayout::create()->setGap(5.f));

    const char* tabNames[] = {"Player", "Level", "Display", "Bypass"};
    for (int i = 0; i < 4; i++) {
        auto btn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(tabNames[i], "goldFont.fnt", "GJ_button_01.png", 0.7f),
            this,
            menu_selector(ModMenuPopup::onTabSwitch)
        );
        btn->setTag(i);
        tabMenu->addChild(btn);
    }
    tabMenu->updateLayout();
    m_mainLayer->addChildAtPosition(tabMenu, Anchor::Top, ccp(0, -35));

    // Toggle list area
    m_toggleMenu = CCMenu::create();
    m_toggleMenu->setContentSize(ccp(340, 200));
    m_toggleMenu->setLayout(
        ColumnLayout::create()
            ->setGap(5.f)
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setCrossAxisLineAlignment(AxisAlignment::Start)
    );
    m_mainLayer->addChildAtPosition(m_toggleMenu, Anchor::Center, ccp(0, -15));

    showTab(0);
    return true;
}

void ModMenuPopup::showTab(int tab) {
    m_currentTab = tab;
    m_toggleMenu->removeAllChildren();

    switch (tab) {
        case 0: // Player
            addToggle("Noclip", "noclip");
            addToggle("No Death Effect", "no-death-effect");
            addToggle("Auto Retry", "auto-retry");
            break;
        case 1: // Level
            addToggle("Instant Complete", "instant-complete");
            addToggle("Practice Music Hack", "practice-music");
            addToggle("No Pause on Unfocus", "no-pause-unfocus");
            addToggle("Auto Play", "auto-play");
            break;
        case 2: // Display
            addToggle("Enable Speed Hack", "speedhack-enabled");
            addSpeedSlider();
            addToggle("Show Accurate Percent", "show-percent");
            addToggle("No Particles", "no-particles");
            break;
        case 3: // Bypass
            addToggle("No Transition", "no-transition");
            addToggle("Unlock All Icons", "unlock-all-icons");
            addToggle("Unlock All Colors", "unlock-all-colors");
            addToggle("Safe Mode", "safe-mode");
            break;
    }

    m_toggleMenu->updateLayout();
}

void ModMenuPopup::addToggle(const char* label, const char* settingKey) {
    auto row = CCNode::create();
    row->setContentSize(ccp(320, 30));
    row->setAnchorPoint(ccp(0.5f, 0.5f));

    auto text = CCLabelBMFont::create(label, "bigFont.fnt");
    text->setScale(0.45f);
    text->setAnchorPoint(ccp(0, 0.5f));
    text->setPosition(ccp(10, 15));
    row->addChild(text);

    bool enabled = Mod::get()->getSettingValue<bool>(settingKey);

    auto toggler = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(ModMenuPopup::onToggle),
        0.6f
    );
    toggler->toggle(enabled);
    toggler->setUserObject(CCString::create(settingKey));
    toggler->setPosition(ccp(290, 15));

    auto toggleMenu = CCMenu::create();
    toggleMenu->setPosition(ccp(0, 0));
    toggleMenu->setContentSize(row->getContentSize());
    toggleMenu->addChild(toggler);
    row->addChild(toggleMenu);

    m_toggleMenu->addChild(row);
}

void ModMenuPopup::addSpeedSlider() {
    auto row = CCNode::create();
    row->setContentSize(ccp(320, 30));
    row->setAnchorPoint(ccp(0.5f, 0.5f));

    double speed = Mod::get()->getSettingValue<double>("speedhack");
    auto speedStr = fmt::format("Speed: {:.1f}x", speed);

    auto text = CCLabelBMFont::create(speedStr.c_str(), "bigFont.fnt");
    text->setScale(0.4f);
    text->setAnchorPoint(ccp(0, 0.5f));
    text->setPosition(ccp(10, 15));
    text->setTag(999);
    row->addChild(text);

    auto slider = Slider::create(this, menu_selector(ModMenuPopup::onSpeedSlider), 0.7f);
    float normalized = static_cast<float>((speed - 0.1) / (5.0 - 0.1));
    slider->setValue(normalized);
    slider->setPosition(ccp(220, 15));
    row->addChild(slider);

    m_toggleMenu->addChild(row);
}

void ModMenuPopup::onTabSwitch(CCObject* sender) {
    int tab = sender->getTag();
    showTab(tab);
}

void ModMenuPopup::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    auto key = static_cast<CCString*>(toggler->getUserObject())->getCString();
    bool newVal = !toggler->isToggled();
    Mod::get()->setSettingValue<bool>(key, newVal);
}

void ModMenuPopup::onSpeedSlider(CCObject* sender) {
    auto slider = static_cast<SliderThumb*>(sender);
    float val = slider->getValue();
    double speed = 0.1 + val * (5.0 - 0.1);
    speed = std::round(speed * 20.0) / 20.0;
    Mod::get()->setSettingValue<double>("speedhack", speed);
}

void ModMenuPopup::keyDown(cocos2d::enumKeyCodes key) {
    if (key == cocos2d::enumKeyCodes::KEY_Tab) {
        this->onClose(nullptr);
        return;
    }
    Popup::keyDown(key);
}

ModMenuPopup* ModMenuPopup::create() {
    auto ret = new ModMenuPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void ModMenuPopup::toggle() {
    auto scene = CCDirector::get()->getRunningScene();
    if (!scene) return;

    if (auto existing = scene->getChildByID("mod-menu-popup"_spr)) {
        existing->removeFromParent();
        return;
    }

    auto popup = ModMenuPopup::create();
    if (popup) {
        popup->setID("mod-menu-popup"_spr);
        popup->show();
    }
}
