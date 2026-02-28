#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScene.hpp>
#include "ui/ModMenuPopup.hpp"

// Only include keyboard hook on desktop
#ifdef GEODE_IS_DESKTOP
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#endif

using namespace geode::prelude;

// ==================== FLOATING BUTTON (all platforms) ====================
// Adds a small draggable button to every scene for opening the mod menu

static constexpr const char* BUTTON_ID = "mod-menu-btn";

class MenuButton : public CCMenu {
    CCPoint m_startTouch;
    CCPoint m_startPos;
    bool m_dragging = false;

public:
    static MenuButton* create() {
        auto ret = new MenuButton();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() override {
        if (!CCMenu::init()) return false;

        // Create the button sprite - small colored circle with "M"
        auto spr = CircleButtonSprite::createWithSpriteFrameName(
            "geode.loader/settings.png", 1.f,
            CircleBaseColor::DarkPurple, CircleBaseSize::Small
        );
        if (!spr) {
            // Fallback if sprite not found
            spr = CircleButtonSprite::createWithSpriteFrameName(
                "edit_eRotBtn_001.png", 1.f,
                CircleBaseColor::DarkPurple, CircleBaseSize::Small
            );
        }

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MenuButton::onMenuBtn)
        );
        btn->setScale(0.7f);
        this->addChild(btn);

        // Position in top-right
        auto winSize = CCDirector::get()->getWinSize();
        this->setPosition(ccp(winSize.width - 25, winSize.height - 25));

        this->setZOrder(9999);
        this->setID("mod-menu-btn"_spr);

        // Allow touch to pass through on mobile for dragging
        this->setTouchPriority(-512);

        return true;
    }

    void onMenuBtn(CCObject*) {
        if (!m_dragging) {
            ModMenuPopup::toggle();
        }
    }

    // Make the button draggable on mobile
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        if (!CCMenu::ccTouchBegan(touch, event)) return false;
        m_startTouch = touch->getLocation();
        m_startPos = this->getPosition();
        m_dragging = false;
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
        auto pos = touch->getLocation();
        auto delta = pos - m_startTouch;
        if (delta.getLength() > 5.f) {
            m_dragging = true;
            this->setPosition(m_startPos + delta);
        }
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) override {
        if (!m_dragging) {
            CCMenu::ccTouchEnded(touch, event);
        }
        m_dragging = false;
    }
};

// Add button to menu layer
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto btn = MenuButton::create();
        this->addChild(btn);

        return true;
    }
};

// Add button to play layer too
class $modify(ButtonPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        auto btn = MenuButton::create();
        this->addChild(btn);

        return true;
    }
};

// ==================== TAB KEY (desktop only) ====================
#ifdef GEODE_IS_DESKTOP
class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool down, bool repeat, double p3) {
        if (down && !repeat && key == cocos2d::enumKeyCodes::KEY_Tab) {
            ModMenuPopup::toggle();
            return true;
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, p3);
    }
};
#endif
