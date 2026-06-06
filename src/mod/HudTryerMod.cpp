#include "mod/HudTryerMod.h"

#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/mod/RegisterHelper.h"

#include "mc/deps/core/string/HashedString.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include "mc/server/commands/CurrentCmdVersion.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/CommandOriginSystem.h"
#include "mc/world/level/Level.h"

#include <string>
#include <string_view>

namespace {

std::string escapeForJson(std::string_view text) {
    std::string escaped;
    escaped.reserve(text.size() + 16);

    for (char ch : text) {
        switch (ch) {
        case '\\':
            escaped += "\\\\";
            break;
        case '"':
            escaped += "\\\"";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            escaped += ch;
            break;
        }
    }

    return escaped;
}

std::string makeRawTextJson(std::string_view text) {
    return "{\"rawtext\":[{\"text\":\"" + escapeForJson(text) + "\"}]}";
}

std::string quoteSelectorName(std::string_view playerName) {
    return "\"" + escapeForJson(playerName) + "\"";
}

void runHudCommand(CommandOrigin const& origin, Player& player, std::string const& command) {
    auto* level = origin.getLevel();
    if (level == nullptr) {
        hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer][WARN] level is null, command skipped");
        return;
    }

    hud_tryer::HudTryerMod::getInstance().logInfo(
        "[HUD tryer][CMD] target=" + player.getRealName() + " command=" + command
    );

    level->runCommand(
        HashedString(command),
        const_cast<CommandOrigin&>(origin),
        CommandOriginSystem::ActorEventCommandSystem,
        CurrentCmdVersion::Latest
    );
}

Player* getPlayerFromOrigin(CommandOrigin const& origin) {
    auto* entity = origin.getEntity();
    if (entity == nullptr || !entity->isType(ActorType::Player)) {
        return nullptr;
    }

    return static_cast<Player*>(entity); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
}

void sendTitleTimes(CommandOrigin const& origin, Player& player) {
    runHudCommand(origin, player, "title " + quoteSelectorName(player.getRealName()) + " times 10 793000 20");
}

void showActionbar(CommandOrigin const& origin, Player& player) {
    runHudCommand(
        origin,
        player,
        "titleraw " + quoteSelectorName(player.getRealName()) + " actionbar "
            + makeRawTextJson("HUD TRYER\nACTIONBAR\n1234567890\n[] {} <>")
    );
}

void showSubtitle(CommandOrigin const& origin, Player& player) {
    sendTitleTimes(origin, player);
    runHudCommand(
        origin,
        player,
        "titleraw " + quoteSelectorName(player.getRealName()) + " title " + makeRawTextJson("HUD TRYER")
    );
    runHudCommand(
        origin,
        player,
        "titleraw " + quoteSelectorName(player.getRealName()) + " subtitle "
            + makeRawTextJson("SUBTITLE\nline-2\nline-3\n[] {} <>")
    );
}

void showTitle(CommandOrigin const& origin, Player& player) {
    sendTitleTimes(origin, player);
    runHudCommand(
        origin,
        player,
        "titleraw " + quoteSelectorName(player.getRealName()) + " title "
            + makeRawTextJson("HUD TRYER\nTITLE\nline-3\n[] {} <>")
    );
}

void clearHud(CommandOrigin const& origin, Player& player) {
    runHudCommand(origin, player, "title " + quoteSelectorName(player.getRealName()) + " clear");
}

void resetHud(CommandOrigin const& origin, Player& player) {
    runHudCommand(origin, player, "title " + quoteSelectorName(player.getRealName()) + " reset");
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
            showActionbar(origin, *player);
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
            showSubtitle(origin, *player);
            output.success("Sent subtitle test to {}.", player->getRealName());
        });

    command.overload().text("title").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer] /hudtry title by " + player->getRealName());
        showTitle(origin, *player);
        output.success("Sent title test to {}.", player->getRealName());
    });

    command.overload().text("all").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer] /hudtry all by " + player->getRealName());
        showActionbar(origin, *player);
        showSubtitle(origin, *player);
        showTitle(origin, *player);
        output.success("Sent all HUD tests to {}.", player->getRealName());
    });

    command.overload().text("clear").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer] /hudtry clear by " + player->getRealName());
        clearHud(origin, *player);
        output.success("Cleared HUD text for {}.", player->getRealName());
    });

    command.overload().text("reset").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        hud_tryer::HudTryerMod::getInstance().logInfo("[HUD tryer] /hudtry reset by " + player->getRealName());
        resetHud(origin, *player);
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
