#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CCTransitionFade.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include "Hacks.hpp"

using namespace geode::prelude;

// ==================== NO TRANSITION ====================
class $modify(CCTransitionFade) {
    static CCTransitionFade* create(float duration, CCScene* scene) {
        if (Hacks::isEnabled("no-transition")) {
            return CCTransitionFade::create(0.0f, scene);
        }
        return CCTransitionFade::create(duration, scene);
    }
};

// ==================== UNLOCK ALL ====================
class $modify(UnlockManager, GameManager) {
    bool isIconUnlocked(int iconID, IconType type) {
        if (Hacks::isEnabled("unlock-all-icons")) return true;
        return GameManager::isIconUnlocked(iconID, type);
    }

    bool isColorUnlocked(int colorID, UnlockType type) {
        if (Hacks::isEnabled("unlock-all-colors")) return true;
        return GameManager::isColorUnlocked(colorID, type);
    }
};

// ==================== COPY HACK ====================
class $modify(CopyLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (Hacks::isEnabled("copy-hack") && level) {
            level->m_isEditable = true;
        }

        return true;
    }
};

// ==================== VERIFY BYPASS ====================
class $modify(BypassEditLayer, EditLevelLayer) {
    void onShare(CCObject* sender) {
        if (Hacks::isEnabled("verify-bypass")) {
            if (m_level) {
                m_level->m_isVerifiedRaw = true;
            }
        }
        EditLevelLayer::onShare(sender);
    }
};
