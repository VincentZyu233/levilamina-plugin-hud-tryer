#include "mod/MyMod.h"

#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/mod/RegisterHelper.h"

#include "mc/deps/core/string/HashedString.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/server/commands/CommandPermissionLevel.h"
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

void runHudCommand(CommandOrigin const& origin, std::string const& command) {
    auto* level = origin.getLevel();
    if (level == nullptr) {
        return;
    }

    level->asServer().runCommand(
        HashedString(command),
        const_cast<CommandOrigin&>(origin),
        CommandOriginSystem::CurrentEntity,
        CurrentCmdVersion::CurrentVersion()
    );
}

Player* getPlayerFromOrigin(CommandOrigin const& origin) {
    auto* entity = origin.getEntity();
    if (entity == nullptr || !entity->isType(ActorType::Player)) {
        return nullptr;
    }

    return static_cast<Player*>(entity); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
}

void sendTitleTimes(CommandOrigin const& origin) {
    runHudCommand(origin, "title @s times 10 70 20");
}

void showActionbar(CommandOrigin const& origin) {
    runHudCommand(
        origin,
        "titleraw @s actionbar " + makeRawTextJson("HUD TRYER\nACTIONBAR\n1234567890\n[] {} <>")
    );
}

void showSubtitle(CommandOrigin const& origin) {
    sendTitleTimes(origin);
    runHudCommand(origin, "titleraw @s title " + makeRawTextJson("HUD TRYER"));
    runHudCommand(
        origin,
        "titleraw @s subtitle " + makeRawTextJson("SUBTITLE\nline-2\nline-3\n[] {} <>")
    );
}

void showTitle(CommandOrigin const& origin) {
    sendTitleTimes(origin);
    runHudCommand(
        origin,
        "titleraw @s title " + makeRawTextJson("HUD TRYER\nTITLE\nline-3\n[] {} <>")
    );
}

void clearHud(CommandOrigin const& origin) { runHudCommand(origin, "title @s clear"); }

void resetHud(CommandOrigin const& origin) { runHudCommand(origin, "title @s reset"); }

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

            showActionbar(origin);
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

            showSubtitle(origin);
            output.success("Sent subtitle test to {}.", player->getRealName());
        });

    command.overload().text("title").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        showTitle(origin);
        output.success("Sent title test to {}.", player->getRealName());
    });

    command.overload().text("all").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        showActionbar(origin);
        showSubtitle(origin);
        showTitle(origin);
        output.success("Sent all HUD tests to {}.", player->getRealName());
    });

    command.overload().text("clear").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        clearHud(origin);
        output.success("Cleared HUD text for {}.", player->getRealName());
    });

    command.overload().text("reset").execute([](CommandOrigin const& origin, CommandOutput& output) {
        auto* player = getPlayerFromOrigin(origin);
        if (player == nullptr) {
            output.error("This command can only be used by a player.");
            return;
        }

        resetHud(origin);
        output.success("Reset HUD state for {}.", player->getRealName());
    });
}

} // namespace

namespace hud_tryer {

MyMod& MyMod::getInstance() {
    static MyMod instance;
    return instance;
}

bool MyMod::load() {
    getSelf().getLogger().debug("Loading...");
    return true;
}

bool MyMod::enable() {
    getSelf().getLogger().debug("Enabling...");
    registerHudTryCommand();
    return true;
}

bool MyMod::disable() {
    getSelf().getLogger().debug("Disabling...");
    return true;
}

} // namespace hud_tryer

LL_REGISTER_MOD(hud_tryer::MyMod, hud_tryer::MyMod::getInstance());
