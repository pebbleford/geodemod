#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "Hacks.hpp"

using namespace geode::prelude;

// ==================== NOCLIP + AUTO RETRY + SAFE MODE ====================
class $modify(NoclipPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (Hacks::isEnabled("noclip")) return;

        PlayLayer::destroyPlayer(player, object);

        if (Hacks::isEnabled("auto-retry") && player == m_player1) {
            this->scheduleOnce(
                schedule_selector(NoclipPlayLayer::doAutoRetry), 0.4f
            );
        }
    }

    void doAutoRetry(float dt) {
        this->resetLevel();
    }

    void levelComplete() {
        bool cheating = Hacks::isEnabled("noclip") ||
                        Hacks::isEnabled("speedhack-enabled") ||
                        Hacks::isEnabled("instant-complete") ||
                        Hacks::isEnabled("auto-play") ||
                        Hacks::isEnabled("anticheat-bypass");

        if (cheating && (Hacks::isEnabled("safe-mode") || Hacks::isEnabled("auto-safe-mode"))) {
            this->m_isTestMode = true;
        }
        PlayLayer::levelComplete();
    }

    void pauseGame(bool unfocused) {
        if (unfocused && Hacks::isEnabled("no-pause-unfocus")) return;
        if (Hacks::isEnabled("ignore-esc") && !unfocused) return;
        PlayLayer::pauseGame(unfocused);
    }
};

// ==================== PLAYER VISUAL MODS ====================
class $modify(ModPlayerObject, PlayerObject) {
    void playerDestroyed(bool noEffects) {
        if (Hacks::isEnabled("no-death-effect")) {
            PlayerObject::playerDestroyed(true);
            return;
        }
        PlayerObject::playerDestroyed(noEffects);
    }

    void setVisible(bool visible) {
        if (Hacks::isEnabled("hide-player")) {
            PlayerObject::setVisible(false);
            return;
        }
        PlayerObject::setVisible(visible);
    }

    void update(float dt) {
        PlayerObject::update(dt);

        // No glow
        if (Hacks::isEnabled("no-glow")) {
            m_hasGlow = false;
        }

        // Solid wave trail
        if (Hacks::isEnabled("solid-wave-trail") && m_waveTrail) {
            m_waveTrail->setOpacity(255);
        }

        // Always trail
        if (Hacks::isEnabled("always-trail") && m_regularTrail) {
            m_regularTrail->setVisible(true);
        }

        // No trail
        if (Hacks::isEnabled("no-trail") && m_regularTrail) {
            m_regularTrail->setVisible(false);
        }

        // No robot/ship fire
        if (Hacks::isEnabled("no-robot-fire") || Hacks::isEnabled("no-ship-fire")) {
            if (m_ghostTrail) {
                m_ghostTrail->setVisible(false);
            }
        }

        // Freeze player
        if (Hacks::isEnabled("freeze-player")) {
            this->setPositionX(this->getPositionX());
        }
    }

    // Jump hack - allow holding to jump repeatedly
    bool pushButton(PlayerButton button) {
        return PlayerObject::pushButton(button);
    }
};

// ==================== JUMP HACK ====================
class $modify(JumpGameLayer, GJBaseGameLayer) {
    struct Fields {
        float m_jumpTimer = 0.f;
    };

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        if (Hacks::isEnabled("jump-hack") && m_player1) {
            m_fields->m_jumpTimer += dt;
            if (m_fields->m_jumpTimer > 0.06f) {
                m_fields->m_jumpTimer = 0.f;
                // Re-trigger jump if button is held
                m_player1->releaseButton(PlayerButton::Jump);
                m_player1->pushButton(PlayerButton::Jump);
            }
        }
    }
};
