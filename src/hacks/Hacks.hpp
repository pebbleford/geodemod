#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace Hacks {
    inline bool isEnabled(const char* key) {
        return Mod::get()->getSavedValue<bool>(key, false);
    }

    inline void setEnabled(const char* key, bool val) {
        Mod::get()->setSavedValue<bool>(key, val);
    }

    inline void toggle(const char* key) {
        setEnabled(key, !isEnabled(key));
    }

    inline float getSpeed() {
        return Mod::get()->getSavedValue<float>("speedhack-value", 1.0f);
    }

    inline void setSpeed(float val) {
        Mod::get()->setSavedValue<float>("speedhack-value", val);
    }
}
