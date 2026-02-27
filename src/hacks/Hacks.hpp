#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace Hacks {
    inline bool isEnabled(const char* key) {
        return Mod::get()->getSettingValue<bool>(key);
    }

    inline double getFloat(const char* key) {
        return Mod::get()->getSettingValue<double>(key);
    }

    inline void toggle(const char* key) {
        bool current = Mod::get()->getSettingValue<bool>(key);
        Mod::get()->setSettingValue<bool>(key, !current);
    }
}
