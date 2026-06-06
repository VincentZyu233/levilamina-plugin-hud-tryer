#include "mod/HudTryerMod.h"

#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/mod/RegisterHelper.h"

#include "mc/network/packet/SetTitlePacket.h"
#include "mc/network/packet/SetTitlePacketPayload.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/actor/player/Player.h"

#include <string>

namespace {

Player* getPlayerFromOrigin(CommandOrigin const& origin) {
    auto* entity = origin.getEntity();
    if (entity == nullptr || !entity->isType(ActorType::Player)) {
        return nullptr;
    }

    return static_cast<Player*>(entity); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
}

std::string sanitizeForLog(std::string text) {
    for (char& ch : text) {
        if (ch == '\n' || ch == '\r') {
            ch = '|';
        }
    }
    return text;
}

void sendHudPacket(Player& player, SetTitlePacketPayload&& payload, std::string const& debugLabel) {
    auto& mod = hud_tryer::HudTryerMod::getInstance();
    mod.logInfo("[HUD tryer][PKT] target=" + player.getRealName() + " " + debugLabel);

    SetTitlePacket packet(std::move(payload));
    player.sendNetworkPacket(packet);
}

void sendTitleTimes(Player& player, int fadeIn, int stay, int fadeOut) {
    sendHudPacket(
        player,
        SetTitlePacketPayload(fadeIn, stay, fadeOut),
        "type=Times fadeIn=" + std::to_string(fadeIn) + " stay=" + std::to_string(stay)
            + " fadeOut=" + std::to_string(fadeOut)
    );
}

void sendHudTextPacket(Player& player, SetTitlePacketPayload::TitleType type, std::string const& text) {
    sendHudPacket(
        player,
        SetTitlePacketPayload(type, text, std::nullopt),
        "type=" + std::to_string(static_cast<int>(type)) + " text=" + sanitizeForLog(text)
    );
}

void showActionbar(Player& player) {
    sendHudTextPacket(player, SetTitlePacketPayload::TitleType::Actionbar, "HUD TRYER ACTIONBAR 1234567890");
}

void showSubtitle(Player& player) {
    sendTitleTimes(player, 10, 793000, 20);
    sendHudTextPacket(player, SetTitlePacketPayload::TitleType::Title, "HUD TRYER");
    sendHudTextPacket(player, SetTitlePacketPayload::TitleType::Subtitle, "SUBTITLE line-2 line-3");
}

void showTitle(Player& player) {
    sendTitleTimes(player, 10, 793000, 20);
    sendHudTextPacket(player, SetTitlePacketPayload::TitleType::Title, "HUD TRYER TITLE line-3");
}

void clearHud(Player& player) {
    sendHudPacket(player, SetTitlePacketPayload(SetTitlePacketPayload::TitleType::Clear), "type=Clear");
}

void resetHud(Player& player) {
    sendHudPacket(player, SetTitlePacketPayload(SetTitlePacketPayload::TitleType::Reset), "type=Reset");
}

void sendUsage(CommandOutput& output) {
    output.success("Usage: /hudtry <actionbar|subtitle|title|all|clear|reset>");
}

void registerHudTryCommand() {
    auto& command = ll::command::CommandRegistrar::getInstance(false).getOrCreateCommand(
        "hudtry",
        "Try Bedrock HUD layers: actionbar, subtitle, title.",
        CommandPermissionLevel::Any
    );

    command.overload().execute([](CommandOrigin const&, CommandOutput& output) { sendUsage(output); });

    command.overload()
        .text("actionbar")
        .execute([](CommandOrigin const& origin, CommandOutput& output) {
            auto* player = getPlayerFromOrigin(origin);
            if (player == nullptr) {
                output.error("This command can only be used by a player.");
                return;
            }

            hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer] /hudtry actionbar by " + player->getRealName());
            showActionbar(*player);
            output.success("Sent actionbar test to {}.", player->getRealName());
        });

    command.overload()
        .text("subtitle")
        .execute([](CommandOrigin const& origin, CommandOutput& output) {
            auto* player = getPlayerFromOrigin(origin);
            if (player == nullptr) {
                output.error("This command can only be used by a player.");
                return;
            }

            hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer] /hudtry subtitle by " + player->getRealName());
            showSubtitle(*player);
            output.success("Sent subtitle test to {}.", player->getRealName());
        });

    command.overload().text("title").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer] /hudtry title by " + player->getRealName());
        showTitle(*player);
        output.success("Sent title test to {}.", player->getRealName());
    });

    command.overload().text("all").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer] /hudtry all by " + player->getRealName());
        showActionbar(*player);
        showSubtitle(*player);
        showTitle(*player);
        output.success("Sent all HUD tests to {}.", player->getRealName());
    });

    command.overload().text("clear").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer] /hudtry clear by " + player->getRealName());
        clearHud(*player);
        output.success("Cleared HUD text for {}.", player->getRealName());
    });

    command.overload().text("reset").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer] /hudtry reset by " + player->getRealName());
        resetHud(*player);
        output.success("Reset HUD state for {}.", player->getRealName());
    });
}

} // namespace

namespace hud_tryer {

HudTryerMod& HudTryerMod::getInstance() {
    static HudTryerMod instance;
    return instance;
}

void HudTryerMod::logInfo(std::string const& message) const { getSelf().getLogger().info(message); }

bool HudTryerMod::load() {
    getSelf().getLogger().debug("Loading...");
    logInfo("[HUD tryer] load()");
    return true;
}

bool HudTryerMod::enable() {
    getSelf().getLogger().debug("Enabling...");
    logInfo("[HUD tryer] enable() registering /hudtry");
    registerHudTryCommand();
    return true;
}

bool HudTryerMod::disable() {
    getSelf().getLogger().debug("Disabling...");
    logInfo("[HUD tryer] disable()");
    return true;
}

} // namespace hud_tryer

LL_REGISTER_MOD(hud_tryer::HudTryerMod, hud_tryer::HudTryerMod::getInstance());
