#include "ModMenuPopup.hpp"
#include "../hacks/Hacks.hpp"

static const std::vector<HackItem> playerHacks = {
    {"Noclip", "noclip", "Pass through obstacles without dying"},
    {"No Death Effect", "no-death-effect", "Skip the death explosion animation"},
    {"Auto Retry", "auto-retry", "Auto restart on death"},
    {"Jump Hack", "jump-hack", "Hold to jump infinitely"},
    {"Hide Player", "hide-player", "Make your icon invisible"},
    {"No Trail", "no-trail", "Disable regular trail"},
    {"Always Trail", "always-trail", "Force trail to always show"},
    {"Solid Wave Trail", "solid-wave-trail", "Make wave trail solid"},
    {"No Glow", "no-glow", "Remove player glow effect"},
    {"No Robot Fire", "no-robot-fire", "Remove robot boost fire"},
    {"No Ship Fire", "no-ship-fire", "Remove ship boost fire"},
    {"Freeze Player", "freeze-player", "Freeze player in place"},
};

static const std::vector<HackItem> levelHacks = {
    {"Instant Complete", "instant-complete", "Complete any level instantly"},
    {"Auto Play", "auto-play", "Auto clicks to beat the level"},
    {"Practice Music", "practice-music", "Normal music in practice mode"},
    {"No Pause on Unfocus", "no-pause-unfocus", "Don't pause when alt-tabbing"},
    {"Ignore ESC", "ignore-esc", "ESC won't pause the level"},
    {"No Camera Shake", "no-camera-shake", "Disable level camera shake"},
    {"No Portal Flash", "no-portal-flash", "Disable portal entry flash"},
    {"No Mirror", "no-mirror", "Disable mirror portals"},
    {"Force Platformer", "force-platformer", "Force platformer mode"},
    {"Layout Mode", "layout-mode", "Hide decorations, show only gameplay"},
    {"No Checkpoints", "no-checkpoints", "Disable checkpoints in practice"},
    {"Auto Safe Mode", "auto-safe-mode", "Auto enable safe mode when cheating"},
};

static const std::vector<HackItem> visualHacks = {
    {"Speed Hack", "speedhack-enabled", "Enable game speed multiplier"},
    {"Show Accurate %", "show-percent", "Show precise level percentage"},
    {"FPS Counter", "fps-counter", "Show FPS in top left corner"},
    {"No Particles", "no-particles", "Disable particles for performance"},
    {"No BG", "no-bg", "Hide the background"},
    {"No Ground", "no-ground", "Hide the ground layer"},
    {"No Shaders", "no-shaders", "Disable shader effects"},
    {"No Glow Objects", "no-glow-objects", "Remove glow from all objects"},
    {"Show Hitboxes", "show-hitboxes", "Show object collision hitboxes"},
    {"Show Attempt Count", "show-attempts", "Show attempt count on screen"},
    {"Show CPS", "show-cps", "Show clicks per second"},
    {"Pulse Menu", "pulse-menu", "Menu icon pulses to the music"},
};

static const std::vector<HackItem> bypassHacks = {
    {"No Transition", "no-transition", "Skip scene fade animations"},
    {"Unlock All Icons", "unlock-all-icons", "Unlock every icon"},
    {"Unlock All Colors", "unlock-all-colors", "Unlock every color"},
    {"Copy Hack", "copy-hack", "Copy any online level"},
    {"Custom Obj Bypass", "custom-obj-bypass", "Bypass custom object limit"},
    {"Text Length Bypass", "text-bypass", "Bypass text character limits"},
    {"Slider Bypass", "slider-bypass", "Bypass editor slider limits"},
    {"Verify Bypass", "verify-bypass", "Bypass level verify requirement"},
    {"Main Level Bypass", "main-levels-bypass", "Access locked main levels"},
    {"Treasure Room Bypass", "treasure-bypass", "Access locked treasure rooms"},
    {"Safe Mode", "safe-mode", "Prevent cheated scores submitting"},
    {"Anticheat Bypass", "anticheat-bypass", "Bypass anticheat detection"},
};

// ============ COLORS ============
static const ccColor4B kBgRow     = {50, 50, 70, 200};
static const ccColor4B kBgRowAlt  = {55, 55, 75, 200};
static const ccColor3B kGreen     = {80, 200, 120};
static const ccColor3B kRed       = {200, 80, 80};
static const ccColor3B kTextWhite = {240, 240, 245};
static const ccColor3B kTextDim   = {160, 160, 180};

bool ModMenuPopup::init() {
    if (!Popup::init(420.f, 290.f, "GJ_square01.png"))
        return false;

    auto winSize = CCDirector::get()->getWinSize();

    // Dark semi-transparent background on the popup bg
    m_bgSprite->setColor({25, 25, 35});
    m_bgSprite->setOpacity(240);

    // ====== TITLE BAR ======
    auto titleBar = CCLayerColor::create({20, 20, 30, 255});
    titleBar->setContentSize(ccp(400, 32));
    titleBar->setAnchorPoint(ccp(0.5f, 1.f));
    titleBar->ignoreAnchorPointForPosition(false);
    m_mainLayer->addChildAtPosition(titleBar, Anchor::Top, ccp(0, -8));

    m_titleLabel = CCLabelBMFont::create("GD MOD MENU", "goldFont.fnt");
    m_titleLabel->setScale(0.55f);
    m_titleLabel->setPosition(ccp(200, 16));
    titleBar->addChild(m_titleLabel);

    // Active hacks count
    m_activeCountLabel = CCLabelBMFont::create("0 active", "chatFont.fnt");
    m_activeCountLabel->setScale(0.55f);
    m_activeCountLabel->setColor(kGreen);
    m_activeCountLabel->setAnchorPoint(ccp(1.f, 0.5f));
    m_activeCountLabel->setPosition(ccp(385, 16));
    titleBar->addChild(m_activeCountLabel);

    // ====== TAB BAR ======
    m_tabBar = CCMenu::create();
    m_tabBar->setContentSize(ccp(400, 28));
    m_tabBar->setLayout(
        RowLayout::create()
            ->setGap(3.f)
            ->setAxisAlignment(AxisAlignment::Center)
    );

    struct TabInfo { const char* label; ccColor3B color; };
    TabInfo tabs[] = {
        {"Player", {120, 180, 255}},
        {"Level", {255, 180, 80}},
        {"Visual", {180, 120, 255}},
        {"Bypass", {255, 100, 100}},
    };

    for (int i = 0; i < 4; i++) {
        auto spr = ButtonSprite::create(
            tabs[i].label, "bigFont.fnt", "GJ_button_04.png", 0.65f
        );
        spr->setScale(0.75f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ModMenuPopup::onTabSwitch));
        btn->setTag(i);
        m_tabBar->addChild(btn);
    }
    m_tabBar->updateLayout();
    m_mainLayer->addChildAtPosition(m_tabBar, Anchor::Top, ccp(0, -50));

    // ====== SCROLL AREA ======
    m_scrollLayer = ScrollLayer::create(ccp(390, 195));
    m_scrollLayer->setAnchorPoint(ccp(0.5f, 0.5f));
    m_scrollLayer->ignoreAnchorPointForPosition(false);
    m_mainLayer->addChildAtPosition(m_scrollLayer, Anchor::Center, ccp(0, -22));

    showTab(0);
    updateActiveCount();

    return true;
}

void ModMenuPopup::showTab(int tab) {
    m_currentTab = tab;
    m_togglers.clear();

    auto content = m_scrollLayer->m_contentLayer;
    content->removeAllChildren();

    const std::vector<HackItem>* hacks = nullptr;
    switch (tab) {
        case 0: hacks = &playerHacks; break;
        case 1: hacks = &levelHacks; break;
        case 2: hacks = &visualHacks; break;
        case 3: hacks = &bypassHacks; break;
    }
    if (!hacks) return;

    float rowHeight = 32.f;
    float totalHeight = hacks->size() * rowHeight;

    // Add speed slider if on visual tab
    if (tab == 2) totalHeight += rowHeight;

    // Make content layer tall enough to scroll
    float minHeight = 195.f;
    if (totalHeight < minHeight) totalHeight = minHeight;
    content->setContentSize(ccp(390, totalHeight));

    float y = totalHeight;
    int idx = 0;

    for (auto& hack : *hacks) {
        y -= rowHeight;
        auto row = createToggleRow(hack.name, hack.key, hack.desc);
        row->setPosition(ccp(0, y));

        // Alternate row colors
        auto bg = CCLayerColor::create(idx % 2 == 0 ? kBgRow : kBgRowAlt);
        bg->setContentSize(ccp(390, rowHeight));
        bg->setPosition(ccp(0, y));
        content->addChild(bg, -1);

        content->addChild(row);
        idx++;

        // Insert speed slider right after "Speed Hack" toggle
        if (tab == 2 && std::string(hack.key) == "speedhack-enabled") {
            y -= rowHeight;
            auto sliderRow = createSliderRow("Speed", Hacks::getSpeed(), 0.1f, 5.0f);
            sliderRow->setPosition(ccp(0, y));

            auto sliderBg = CCLayerColor::create({45, 45, 65, 200});
            sliderBg->setContentSize(ccp(390, rowHeight));
            sliderBg->setPosition(ccp(0, y));
            content->addChild(sliderBg, -1);
            content->addChild(sliderRow);
        }
    }

    m_scrollLayer->moveToTop();
}

CCNode* ModMenuPopup::createToggleRow(const char* label, const char* key, const char* desc) {
    auto row = CCNode::create();
    row->setContentSize(ccp(390, 32));

    // Hack name
    auto text = CCLabelBMFont::create(label, "bigFont.fnt");
    text->setScale(0.38f);
    text->setAnchorPoint(ccp(0, 0.5f));
    text->setPosition(ccp(12, 16));
    text->setColor(kTextWhite);
    row->addChild(text);

    // Description
    auto descLabel = CCLabelBMFont::create(desc, "chatFont.fnt");
    descLabel->setScale(0.45f);
    descLabel->setAnchorPoint(ccp(0, 0.5f));
    descLabel->setPosition(ccp(12, 5));
    descLabel->setColor(kTextDim);
    descLabel->setOpacity(160);
    row->addChild(descLabel);

    // Toggle switch
    bool enabled = Hacks::isEnabled(key);

    auto toggler = CCMenuItemToggler::createWithStandardSprites(
        this,
        menu_selector(ModMenuPopup::onToggle),
        0.55f
    );
    toggler->toggle(enabled);
    toggler->setUserObject(CCString::create(key));
    toggler->setPosition(ccp(360, 16));
    m_togglers.push_back(toggler);

    // Status dot
    auto dot = CCSprite::createWithSpriteFrameName("d_circle_02_001.png");
    if (dot) {
        dot->setScale(0.15f);
        dot->setColor(enabled ? kGreen : kRed);
        dot->setPosition(ccp(340, 16));
        dot->setTag(42);
        row->addChild(dot);
    }

    auto menu = CCMenu::create();
    menu->setPosition(ccp(0, 0));
    menu->setContentSize(row->getContentSize());
    menu->addChild(toggler);
    row->addChild(menu);

    return row;
}

CCNode* ModMenuPopup::createSliderRow(const char* label, float value, float min, float max) {
    auto row = CCNode::create();
    row->setContentSize(ccp(390, 32));

    auto speedStr = fmt::format("{}: {:.2f}x", label, value);
    auto text = CCLabelBMFont::create(speedStr.c_str(), "bigFont.fnt");
    text->setScale(0.35f);
    text->setAnchorPoint(ccp(0, 0.5f));
    text->setPosition(ccp(12, 16));
    text->setColor({200, 200, 255});
    text->setTag(998);
    row->addChild(text);

    auto slider = Slider::create(this, menu_selector(ModMenuPopup::onSpeedSlider), 0.6f);
    float normalized = (value - min) / (max - min);
    slider->setValue(normalized);
    slider->setPosition(ccp(280, 16));
    slider->setTag(997);
    row->addChild(slider);

    return row;
}

void ModMenuPopup::onTabSwitch(CCObject* sender) {
    showTab(sender->getTag());
}

void ModMenuPopup::onToggle(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    auto key = static_cast<CCString*>(toggler->getUserObject())->getCString();
    bool newVal = !toggler->isToggled();
    Hacks::setEnabled(key, newVal);
    updateActiveCount();

    // Update dot color
    auto row = toggler->getParent()->getParent();
    if (auto dot = row->getChildByTag(42)) {
        auto spr = static_cast<CCSprite*>(dot);
        spr->setColor(newVal ? kGreen : kRed);
    }
}

void ModMenuPopup::onSpeedSlider(CCObject* sender) {
    auto slider = static_cast<SliderThumb*>(sender);
    float val = slider->getValue();
    float speed = 0.1f + val * (5.0f - 0.1f);
    speed = std::round(speed * 20.0f) / 20.0f;
    Hacks::setSpeed(speed);

    // Update label - walk up to find the row
    auto sliderParent = static_cast<CCNode*>(sender)->getParent();
    if (sliderParent) {
        auto row = sliderParent->getParent();
        if (row) {
            if (auto label = static_cast<CCLabelBMFont*>(row->getChildByTag(998))) {
                auto str = fmt::format("Speed: {:.2f}x", speed);
                label->setString(str.c_str());
            }
        }
    }
}

void ModMenuPopup::keyDown(cocos2d::enumKeyCodes key, double p1) {
    if (key == cocos2d::enumKeyCodes::KEY_Tab) {
        this->onClose(nullptr);
        return;
    }
    Popup::keyDown(key, p1);
}

int ModMenuPopup::countActiveHacks() {
    int count = 0;
    auto allHacks = {&playerHacks, &levelHacks, &visualHacks, &bypassHacks};
    for (auto* list : allHacks) {
        for (auto& h : *list) {
            if (Hacks::isEnabled(h.key)) count++;
        }
    }
    return count;
}

void ModMenuPopup::updateActiveCount() {
    int count = countActiveHacks();
    auto str = fmt::format("{} active", count);
    m_activeCountLabel->setString(str.c_str());
    m_activeCountLabel->setColor(count > 0 ? kGreen : kTextDim);
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
