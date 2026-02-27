#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CCTransitionFade.hpp>
#include "Hacks.hpp"

using namespace geode::prelude;

// ==================== NO TRANSITION ====================
class $modify(CCTransitionFade) {
    static CCTransitionFade* create(float duration, CCScene* scene) {
        if (Hacks::isEnabled("no-transition")) {
            // Use zero duration to skip the fade effect
            return CCTransitionFade::create(0.0f, scene);
        }
        return CCTransitionFade::create(duration, scene);
    }
};

// ==================== UNLOCK ALL ICONS ====================
class $modify(UnlockGameManager, GameManager) {
    bool isIconUnlocked(int iconID, IconType type) {
        if (Hacks::isEnabled("unlock-all-icons")) {
            return true;
        }
        return GameManager::isIconUnlocked(iconID, type);
    }

    // ==================== UNLOCK ALL COLORS ====================
    bool isColorUnlocked(int colorID, UnlockType type) {
        if (Hacks::isEnabled("unlock-all-colors")) {
            return true;
        }
        return GameManager::isColorUnlocked(colorID, type);
    }
};
