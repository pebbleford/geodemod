#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCParticleSystem.hpp>
#include "Hacks.hpp"

using namespace geode::prelude;

// ==================== SPEED HACK ====================
class $modify(CCScheduler) {
    void update(float dt) {
        if (Hacks::isEnabled("speedhack-enabled")) {
            float speed = Hacks::getSpeed();
            if (speed > 0.f) dt *= speed;
        }
        CCScheduler::update(dt);
    }
};

// ==================== HUD OVERLAYS ====================
class $modify(HUDPlayLayer, PlayLayer) {
    struct Fields {
        CCLabelBMFont* m_percentLabel = nullptr;
        CCLabelBMFont* m_fpsLabel = nullptr;
        CCLabelBMFont* m_attemptLabel = nullptr;
        CCLabelBMFont* m_cpsLabel = nullptr;
        float m_fpsTimer = 0.f;
        int m_frameCount = 0;
        float m_cpsTimer = 0.f;
        int m_clickCount = 0;
        int m_currentCps = 0;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        auto winSize = CCDirector::get()->getWinSize();

        // Accurate percent
        if (Hacks::isEnabled("show-percent")) {
            auto label = CCLabelBMFont::create("0.00%", "bigFont.fnt");
            label->setScale(0.35f);
            label->setPosition(ccp(winSize.width / 2, winSize.height - 12));
            label->setOpacity(200);
            label->setZOrder(999);
            label->setID("percent-label"_spr);
            this->addChild(label);
            m_fields->m_percentLabel = label;
        }

        // FPS counter
        if (Hacks::isEnabled("fps-counter")) {
            auto label = CCLabelBMFont::create("0 FPS", "chatFont.fnt");
            label->setScale(0.5f);
            label->setAnchorPoint(ccp(0, 1));
            label->setPosition(ccp(5, winSize.height - 5));
            label->setOpacity(180);
            label->setZOrder(999);
            label->setColor({180, 255, 180});
            label->setID("fps-label"_spr);
            this->addChild(label);
            m_fields->m_fpsLabel = label;
        }

        // Attempt counter
        if (Hacks::isEnabled("show-attempts")) {
            auto label = CCLabelBMFont::create("Attempt 1", "bigFont.fnt");
            label->setScale(0.3f);
            label->setAnchorPoint(ccp(1, 1));
            label->setPosition(ccp(winSize.width - 5, winSize.height - 5));
            label->setOpacity(180);
            label->setZOrder(999);
            label->setColor({255, 200, 120});
            label->setID("attempt-label"_spr);
            this->addChild(label);
            m_fields->m_attemptLabel = label;
        }

        // CPS counter
        if (Hacks::isEnabled("show-cps")) {
            auto label = CCLabelBMFont::create("0 CPS", "chatFont.fnt");
            label->setScale(0.5f);
            label->setAnchorPoint(ccp(0, 1));
            label->setPosition(ccp(5, winSize.height - 20));
            label->setOpacity(180);
            label->setZOrder(999);
            label->setColor({180, 180, 255});
            label->setID("cps-label"_spr);
            this->addChild(label);
            m_fields->m_cpsLabel = label;
        }

        // Layout mode - hide decorations
        if (Hacks::isEnabled("layout-mode")) {
            // Hide background and ground visual elements
            if (m_background) m_background->setVisible(false);
            if (m_groundLayer) m_groundLayer->setVisible(false);
            if (m_groundLayer2) m_groundLayer2->setVisible(false);
        }

        // No BG
        if (Hacks::isEnabled("no-bg") && m_background) {
            m_background->setVisible(false);
        }

        // No ground
        if (Hacks::isEnabled("no-ground")) {
            if (m_groundLayer) m_groundLayer->setVisible(false);
            if (m_groundLayer2) m_groundLayer2->setVisible(false);
        }

        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        // Accurate percent
        if (m_fields->m_percentLabel) {
            float pct = this->getCurrentPercent();
            m_fields->m_percentLabel->setString(
                fmt::format("{:.2f}%", pct).c_str()
            );
        }

        // FPS
        if (m_fields->m_fpsLabel) {
            m_fields->m_frameCount++;
            m_fields->m_fpsTimer += dt;
            if (m_fields->m_fpsTimer >= 0.5f) {
                int fps = static_cast<int>(m_fields->m_frameCount / m_fields->m_fpsTimer);
                m_fields->m_fpsLabel->setString(fmt::format("{} FPS", fps).c_str());
                m_fields->m_fpsTimer = 0.f;
                m_fields->m_frameCount = 0;
            }
        }

        // Attempt count
        if (m_fields->m_attemptLabel) {
            m_fields->m_attemptLabel->setString(
                fmt::format("Att {}", m_jumps).c_str()
            );
        }

        // CPS
        if (m_fields->m_cpsLabel) {
            m_fields->m_cpsTimer += dt;
            if (m_fields->m_cpsTimer >= 1.0f) {
                m_fields->m_currentCps = m_fields->m_clickCount;
                m_fields->m_clickCount = 0;
                m_fields->m_cpsTimer = 0.f;
            }
            m_fields->m_cpsLabel->setString(
                fmt::format("{} CPS", m_fields->m_currentCps).c_str()
            );
        }

        // Show hitboxes
        if (Hacks::isEnabled("show-hitboxes") && m_debugDrawNode) {
            m_debugDrawNode->setVisible(true);
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        if (m_fields->m_cpsLabel) {
            m_fields->m_clickCount = 0;
            m_fields->m_currentCps = 0;
        }
    }
};

// ==================== NO PARTICLES ====================
class $modify(CCParticleSystem) {
    void update(float dt) {
        if (Hacks::isEnabled("no-particles")) return;
        CCParticleSystem::update(dt);
    }
};
