#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include "Hacks.hpp"

using namespace geode::prelude;

// ==================== NOCLIP ====================
// Skip death when player hits an obstacle
class $modify(NoclipPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (Hacks::isEnabled("noclip")) {
            // Don't kill the player - just skip the death call entirely
            return;
        }

        // Auto retry: if enabled and we die, schedule a restart
        if (Hacks::isEnabled("auto-retry")) {
            PlayLayer::destroyPlayer(player, object);
            // Schedule reset after a short delay
            this->scheduleOnce(
                schedule_selector(NoclipPlayLayer::doAutoRetry),
                0.5f
            );
            return;
        }

        PlayLayer::destroyPlayer(player, object);
    }

    void doAutoRetry(float dt) {
        this->resetLevel();
    }

    // Safe mode: prevent cheated completions from submitting
    void levelComplete() {
        if (Hacks::isEnabled("safe-mode")) {
            // Check if any cheats are active
            bool cheating = Hacks::isEnabled("noclip") ||
                           Hacks::isEnabled("speedhack-enabled") ||
                           Hacks::isEnabled("instant-complete") ||
                           Hacks::isEnabled("auto-play");

            if (cheating) {
                // Still show completion but don't verify/submit
                // The game handles this through m_isTestMode
                this->m_isTestMode = true;
            }
        }
        PlayLayer::levelComplete();
    }
};

// ==================== NO DEATH EFFECT ====================
class $modify(PlayerObject) {
    void playerDestroyed(bool noEffects) {
        if (Hacks::isEnabled("no-death-effect")) {
            // Call with noEffects = true to skip the explosion
            PlayerObject::playerDestroyed(true);
            return;
        }
        PlayerObject::playerDestroyed(noEffects);
    }
};
