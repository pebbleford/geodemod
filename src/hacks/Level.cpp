#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "Hacks.hpp"

using namespace geode::prelude;

// ==================== INSTANT COMPLETE ====================
class $modify(InstantPlayLayer, PlayLayer) {
    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        if (Hacks::isEnabled("instant-complete")) {
            // Complete the level right after it loads
            this->scheduleOnce(
                schedule_selector(InstantPlayLayer::doInstantComplete),
                0.5f
            );
        }
    }

    void doInstantComplete(float dt) {
        this->levelComplete();
    }

    // Practice music hack: force normal music in practice mode
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        if (Hacks::isEnabled("practice-music")) {
            // Enable the practice music hack through GameManager
            GameManager::get()->m_practiceMusicSync = true;
        }

        return true;
    }

    // No pause on unfocus
    void pauseGame(bool unfocused) {
        if (unfocused && Hacks::isEnabled("no-pause-unfocus")) {
            // Don't pause when the window loses focus
            return;
        }
        PlayLayer::pauseGame(unfocused);
    }
};

// ==================== AUTO PLAY ====================
// Automatically clicks at the right times
class $modify(AutoPlayLayer, GJBaseGameLayer) {
    struct Fields {
        float m_autoClickTimer = 0.f;
        bool m_isHolding = false;
    };

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        if (!Hacks::isEnabled("auto-play")) {
            if (m_fields->m_isHolding) {
                m_fields->m_isHolding = false;
                if (m_player1) {
                    m_player1->releaseButton(PlayerButton::Jump);
                }
            }
            return;
        }

        if (!m_player1) return;

        // Simple auto-click pattern: click rapidly
        m_fields->m_autoClickTimer += dt;

        // Click every ~0.15 seconds with short hold time
        float clickInterval = 0.15f;
        float holdTime = 0.05f;

        float cycleTime = fmod(m_fields->m_autoClickTimer, clickInterval);

        if (cycleTime < holdTime && !m_fields->m_isHolding) {
            m_fields->m_isHolding = true;
            m_player1->pushButton(PlayerButton::Jump);
        } else if (cycleTime >= holdTime && m_fields->m_isHolding) {
            m_fields->m_isHolding = false;
            m_player1->releaseButton(PlayerButton::Jump);
        }
    }
};
