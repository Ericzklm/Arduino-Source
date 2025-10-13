/*  Game Entry
 *
 *  From: https://github.com/PokemonAutomation/
 *
 */

#ifndef PokemonAutomation_PokemonLA_GameEntry_H
#define PokemonAutomation_PokemonLA_GameEntry_H

#include "NintendoSwitch/Controllers/NintendoSwitch_ProController.h"
#include "NintendoSwitch/NintendoSwitch_ConsoleHandle.h"

namespace PokemonAutomation{
    class ProgramEnvironment;
namespace NintendoSwitch{
namespace PokemonLA{

using namespace std::chrono_literals;


//  From Switch Home menu, reset game and wait until the game menu screen (where
//  "Press A" is displayed to enter the game) is shown.
bool reset_game_to_gamemenu(
    ConsoleHandle& console, ProControllerContext& context,
    bool tolerate_update_menu
);

//  From Switch Home menu, start game and wait until the player character
//  appears in game.
//  post_wait_time: how many ticks to wait after the black screen
//  (shown when loading the map) is over.
bool reset_game_from_home(
    ProgramEnvironment& env,
    ConsoleHandle& console, ProControllerContext& context,
    Milliseconds enter_game_mash,
    Milliseconds enter_game_timeout,
    Milliseconds post_wait_time
);
bool reset_game_from_home(
    ProgramEnvironment& env,
    ConsoleHandle& console, ProControllerContext& context,
    Milliseconds post_wait_time = 1000ms
);




}
}
}
#endif
