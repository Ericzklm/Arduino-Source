/*  Auto Host Tools
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "ClientSource/Connection/BotBase.h"
#include "CommonFramework/Exceptions/FatalProgramException.h"
#include "CommonFramework/Notifications/ProgramNotifications.h"
#include "CommonFramework/VideoPipeline/VideoFeed.h"
#include "CommonFramework/Tools/ProgramEnvironment.h"
#include "CommonFramework/Tools/ConsoleHandle.h"
#include "PokemonSV/Options/PokemonSV_AutoHostOptions.h"
#include "PokemonSV_AutoHostTools.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSV{



void send_host_announcement(
    ProgramEnvironment& env, ConsoleHandle& host_console,
    const std::string& lobby_code, bool show_raid_code,
    const std::string& description,
    EventNotificationOption& NOTIFICATION_RAID_POST
){
    std::vector<std::pair<std::string, std::string>> messages;
    if (!description.empty()){
        messages.emplace_back("Description:", std::move(description));
    }

    VideoSnapshot snapshot = host_console.video().snapshot();
    if (show_raid_code && !lobby_code.empty()){
        messages.emplace_back("Raid Code:", lobby_code);
    }

    send_program_notification(
        env, NOTIFICATION_RAID_POST,
        Color(0),
        "Tera Raid Notification",
        messages, "",
        snapshot
    );
}



TeraFailTracker::TeraFailTracker(
    ProgramEnvironment& env, CancellableScope& scope,
    EventNotificationOption& notification_error_recoverable,
    ConsecutiveFailurePause& consecutive_failure_pause,
    FailurePauseMinutes& failure_pause_minutes
)
    : m_env(env)
    , m_scope(scope)
    , m_notification_error_recoverable(notification_error_recoverable)
    , m_consecutive_failure_pause(consecutive_failure_pause)
    , m_failure_pause_minutes(failure_pause_minutes)
    , m_current_raid_error(false)
{}

void TeraFailTracker::on_raid_start(){
    if (!m_current_raid_error.load(std::memory_order_relaxed)){
        return;
    }
    m_current_raid_error.store(false, std::memory_order_relaxed);

    if (m_consecutive_failures > 0 && !m_completed_one){
        throw FatalProgramException(m_env.logger(), "Failed 1st raid attempt. Will not retry due to risk of ban.");
    }
    size_t fail_threshold = m_consecutive_failure_pause;
    if (m_consecutive_failures >= fail_threshold){
        uint16_t minutes = m_failure_pause_minutes;
        if (minutes == 0){
            throw FatalProgramException(
                m_env.logger(),
                "Failed " + std::to_string(fail_threshold) +  " raid(s) in the row. "
                "Stopping to prevent possible ban."
            );
        }else{
            send_program_recoverable_error_notification(
                m_env, m_notification_error_recoverable,
                "Failed " + std::to_string(fail_threshold) +  " raid(s) in the row. "
                "Pausing program for " + std::to_string(minutes) + " minute(s)."
            );
            WallClock start_time = current_time();
            while (current_time() < start_time + std::chrono::minutes(m_failure_pause_minutes)){
                m_scope.wait_for(std::chrono::seconds(1));
            }
            m_consecutive_failures = 0;
        }
    }
}
void TeraFailTracker::report_successful_raid(){
    m_completed_one = true;
    m_consecutive_failures = 0;
}
void TeraFailTracker::report_raid_error(){
    bool expected = false;
    if (m_current_raid_error.compare_exchange_strong(expected, true)){
        m_consecutive_failures++;
        m_env.log("Consecutive Failure Count: " + std::to_string(m_consecutive_failures), COLOR_RED);
    }
}







}
}
}