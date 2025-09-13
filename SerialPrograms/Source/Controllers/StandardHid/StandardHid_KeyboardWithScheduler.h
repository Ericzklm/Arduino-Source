/*  HID Keyboard Controller (With Scheduler)
 *
 *  From: https://github.com/PokemonAutomation/
 *
 *  This implements most of the Keyboard API using only the controller
 *  state function. It uses SuperscalarScheduler to do this.
 *
 */

#ifndef PokemonAutomation_StandardHid_KeyboardWithScheduler_H
#define PokemonAutomation_StandardHid_KeyboardWithScheduler_H

#include <set>
#include "Common/Cpp/CancellableScope.h"
#include "Controllers/Schedulers/ControllerWithScheduler.h"
#include "StandardHid_Keyboard_ControllerButtons.h"

namespace PokemonAutomation{
namespace StandardHid{



struct KeyboardControllerState{
    std::set<KeyboardKey> keys;
};

class KeyboardCommand : public SchedulerResource{
public:
    KeyboardCommand(KeyboardKey key)
        : SchedulerResource((size_t)key)
    {}
    KeyboardKey key() const{
        return (KeyboardKey)id;
    }
};



class KeyboardControllerWithScheduler : public PokemonAutomation::ControllerWithScheduler{
public:
    using PokemonAutomation::ControllerWithScheduler::ControllerWithScheduler;


public:
    //  Superscalar Commands (the "ssf" framework)

    void issue_key(
        const Cancellable* cancellable,
        Milliseconds delay, Milliseconds hold, Milliseconds cooldown,
        KeyboardKey key
    );
    void issue_keys(
        const Cancellable* cancellable,
        Milliseconds delay, Milliseconds hold, Milliseconds cooldown,
        const std::set<KeyboardKey>& keys
    );
};




}
}
#endif
