#pragma once

#include "ll/api/mod/NativeMod.h"

namespace hud_tryer {

class HudTryerMod {

public:
    static HudTryerMod& getInstance();

    HudTryerMod() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }
    void logInfo(std::string const& message) const;

    bool load();
    bool enable();
    bool disable();

private:
    ll::mod::NativeMod& mSelf;
};

} // namespace hud_tryer
