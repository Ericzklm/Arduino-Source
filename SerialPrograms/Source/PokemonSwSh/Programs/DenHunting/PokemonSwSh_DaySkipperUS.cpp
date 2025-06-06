/*  Day Skipper (US)
 *
 *  From: https://github.com/PokemonAutomation/
 *
 */

#ifdef PA_OFFICIAL

#include "CommonFramework/Notifications/ProgramNotifications.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Superscalar.h"
#include "NintendoSwitch/Programs/NintendoSwitch_DateSkippers.h"
#include "Pokemon/Pokemon_Strings.h"
#include "PokemonSwSh_DaySkipperStats.h"
#include "PokemonSwSh_DaySkipperUS.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{

using namespace Pokemon;


DaySkipperUS_Descriptor::DaySkipperUS_Descriptor()
    : SingleSwitchProgramDescriptor(
        "PokemonSwSh:DaySkipperUS",
        STRING_POKEMON + " SwSh", "Day Skipper (US)",
        "ComputerControl/blob/master/Wiki/Programs/PokemonSwSh/DaySkipperUS.md",
        "A day skipper for US date format that.  (~7100 skips/hour)",
        FeedbackType::NONE,
        AllowCommandsWhenRunning::DISABLE_COMMANDS,
        {
            ControllerFeature::TickPrecise,
            ControllerFeature::NintendoSwitch_ProController,
            ControllerFeature::NintendoSwitch_DateSkip,
        }
    )
{}
std::unique_ptr<StatsTracker> DaySkipperUS_Descriptor::make_stats() const{
    return std::unique_ptr<StatsTracker>(new SkipperStats());
}



DaySkipperUS::DaySkipperUS()
    : SKIPS(
        "<b>Number of Frame Skips:</b>",
        LockMode::LOCK_WHILE_RUNNING,
        10
    )
    , REAL_LIFE_YEAR(
        "<b>Real Life Year:</b>",
        LockMode::LOCK_WHILE_RUNNING,
        std::min(current_year(), (uint16_t)2060),
        2000, 2060
    )
    , NOTIFICATION_PROGRESS_UPDATE("Progress Update", true, false, std::chrono::seconds(3600))
    , NOTIFICATIONS({
        &NOTIFICATION_PROGRESS_UPDATE,
        &NOTIFICATION_PROGRAM_FINISH,
        &NOTIFICATION_ERROR_FATAL,
    })
    , m_advanced_options(
        "<font size=4><b>Advanced Options:</b> You should not need to touch anything below here.</font>"
    )
    , CORRECTION_SKIPS(
        "<b>Auto-Correct Interval:</b><br>Run auto-recovery every this # of skips. Zero disables the auto-corrections.",
        LockMode::LOCK_WHILE_RUNNING,
        1000
    )
{
    PA_ADD_OPTION(SKIPS);
    PA_ADD_OPTION(REAL_LIFE_YEAR);
    PA_ADD_OPTION(NOTIFICATIONS);
    PA_ADD_STATIC(m_advanced_options);
    PA_ADD_OPTION(CORRECTION_SKIPS);
}


void DaySkipperUS::program(SingleSwitchProgramEnvironment& env, ProControllerContext& context){
    if (context->performance_class() != ControllerPerformanceClass::SerialPABotBase_Wired_125Hz){
        throw UserSetupError(
            env.logger(),
            "This program requires a tick precise wired controller."
        );
    }

    SkipperStats& stats = env.current_stats<SkipperStats>();
    stats.total_skips = SKIPS;
    stats.runs++;

    //  Setup globals.
    uint8_t real_life_year = (uint8_t)(
        REAL_LIFE_YEAR < 2000 ?  0 :
        REAL_LIFE_YEAR > 2060 ? 60 : REAL_LIFE_YEAR - 2000
    );
    uint8_t year = 60;
    uint32_t remaining_skips = SKIPS;

    //  Connect
    pbf_press_button(context, BUTTON_ZR, 5, 5);

    //  Setup starting state.
    DateSkippers::init_view(context);
    DateSkippers::rollback_year_full(context, true);
    year = 0;

    uint16_t correct_count = 0;
    while (remaining_skips > 0){
        send_program_status_notification(env, NOTIFICATION_PROGRESS_UPDATE);

        DateSkippers::increment_day(context, true);

        correct_count++;
        year++;
        remaining_skips--;
        stats.issued++;
//        env.log("Skips Remaining: " + tostr_u_commas(remaining_skips));
        env.update_stats();

        if (year >= 60){
            if (real_life_year <= 36){
                DateSkippers::rollback_year_sync(context);
                year = real_life_year;
            }else{
                DateSkippers::rollback_year_full(context, true);
                year = 0;
            }
        }
        if (CORRECTION_SKIPS != 0 && correct_count == CORRECTION_SKIPS){
            correct_count = 0;
            DateSkippers::auto_recovery(context);
        }
    }

    //  Prevent the Switch from sleeping and the time from advancing.
    context.wait_for_all_requests();
    send_program_finished_notification(env, NOTIFICATION_PROGRAM_FINISH);

    pbf_wait(context, 15 * TICKS_PER_SECOND);
    while (true){
        ssf_press_button1(context, BUTTON_A, 15 * TICKS_PER_SECOND);
    }
}



}
}
}
#endif
