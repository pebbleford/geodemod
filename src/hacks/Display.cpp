#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCParticleSystem.hpp>
#include "Hacks.hpp"

using namespace geode::prelude;

// ==================== SPEED HACK ====================
class $modify(CCScheduler) {
    void update(float dt) {
        if (Hacks::isEnabled("speedhack-enabled")) {
            double speed = Hacks::getFloat("speedhack");
            if (speed > 0.0) {
                dt *= static_cast<float>(speed);
            }
        }
        CCScheduler::update(dt);
    }
};

// ==================== SHOW PERCENT & FPS ====================
class $modify(PercentPlayLayer, PlayLayer) {
    struct Fields {
        CCLabelBMFont* m_percentLabel = nullptr;
        CCLabelBMFont* m_fpsLabel = nullptr;
        float m_fpsTimer = 0.f;
        int m_frameCount = 0;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        auto winSize = CCDirector::get()->getWinSize();

        // Accurate percent label
        if (Hacks::isEnabled("show-percent")) {
            auto label = CCLabelBMFont::create("0.00%", "bigFont.fnt");
            label->setScale(0.4f);
            label->setPosition(ccp(winSize.width / 2, winSize.height - 15));
            label->setOpacity(180);
            label->setZOrder(1000);
            label->setID("accurate-percent"_spr);
            this->addChild(label);
            m_fields->m_percentLabel = label;
        }

        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        // Update accurate percent
        if (m_fields->m_percentLabel && Hacks::isEnabled("show-percent")) {
            float percent = this->getCurrentPercent();
            auto str = fmt::format("{:.2f}%", percent);
            m_fields->m_percentLabel->setString(str.c_str());
        }
    }
};

// ==================== NO PARTICLES ====================
class $modify(CCParticleSystem) {
    void update(float dt) {
        if (Hacks::isEnabled("no-particles")) {
            // Don't update particles at all
            return;
        }
        CCParticleSystem::update(dt);
    }
};
