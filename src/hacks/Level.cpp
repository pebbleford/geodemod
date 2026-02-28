#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include "Hacks.hpp"

using namespace geode::prelude;

// ==================== INSTANT COMPLETE + PRACTICE MUSIC ====================
class $modify(LevelPlayLayer, PlayLayer) {
    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        if (Hacks::isEnabled("instant-complete")) {
            this->scheduleOnce(
                schedule_selector(LevelPlayLayer::doInstantComplete), 0.5f
            );
        }
    }

    void doInstantComplete(float dt) {
        this->levelComplete();
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;

        if (Hacks::isEnabled("practice-music")) {
            this->m_practiceMusicSync = true;
        }

        return true;
    }

    // No checkpoints in practice
    CheckpointObject* markCheckpoint() {
        if (Hacks::isEnabled("no-checkpoints")) return nullptr;
        return PlayLayer::markCheckpoint();
    }
};

// ==================== AUTO PLAY ====================
class $modify(AutoPlayLayer, GJBaseGameLayer) {
    struct Fields {
        float m_autoTimer = 0.f;
        bool m_holding = false;
    };

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        if (!Hacks::isEnabled("auto-play") || !m_player1) {
            if (m_fields->m_holding) {
                m_fields->m_holding = false;
                m_player1->releaseButton(PlayerButton::Jump);
            }
            return;
        }

        m_fields->m_autoTimer += dt;
        float cycle = fmod(m_fields->m_autoTimer, 0.12f);

        if (cycle < 0.04f && !m_fields->m_holding) {
            m_fields->m_holding = true;
            m_player1->pushButton(PlayerButton::Jump);
        } else if (cycle >= 0.04f && m_fields->m_holding) {
            m_fields->m_holding = false;
            m_player1->releaseButton(PlayerButton::Jump);
        }
    }
};

// ==================== COPY HACK ====================
class $modify(GJGameLevel) {
    void savePercentage(int percent, bool isPractice, int clicks, int attempts, bool isChkValid) {
        GJGameLevel::savePercentage(percent, isPractice, clicks, attempts, isChkValid);
    }

    gd::string getAudioFileName() {
        return GJGameLevel::getAudioFileName();
    }
};
