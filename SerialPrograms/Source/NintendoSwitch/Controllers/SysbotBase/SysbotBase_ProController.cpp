/*  Nintendo Switch Pro Controller (SysbotBase)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Cpp/Exceptions.h"
#include "CommonFramework/GlobalSettingsPanel.h"
#include "CommonFramework/Options/Environment/PerformanceOptions.h"
#include "CommonFramework/Options/Environment/ThemeSelectorOption.h"
#include "Controllers/ControllerTypeStrings.h"
#include "SysbotBase_ProController.h"

#include "Common/Cpp/Concurrency/SpinPause.h"

//#include <iostream>
//using std::cout;
//using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{



ProController_SysbotBase::ProController_SysbotBase(
    Logger& logger,
    SysbotBase::TcpSysbotBase_Connection& connection,
    const ControllerRequirements& requirements
)
    : ControllerWithScheduler(logger)
    , m_connection(connection)
    , m_stopping(false)
    , m_replace_on_next(false)
    , m_command_queue(QUEUE_SIZE)
    , m_is_active(false)
{
    if (!connection.is_ready()){
        return;
    }

    //  Check compatibility.

    ControllerModeStatus mode_status = connection.controller_mode_status();
    auto iter = mode_status.supported_controllers.find(ControllerType::NintendoSwitch_WiredProController);

    std::string missing_feature;
    do{
        if (iter == mode_status.supported_controllers.end()){
            missing_feature = "NintendoSwitch_WiredProController";
            break;
        }

        missing_feature = requirements.check_compatibility(iter->second);

        if (PreloadSettings::instance().DEVELOPER_MODE &&
            missing_feature == CONTROLLER_FEATURE_STRINGS.get_string(ControllerFeature::TickPrecise)
        ){
            logger.log("Bypassing Missing Requirement: " + missing_feature, COLOR_RED);
            missing_feature.clear();
        }

        if (missing_feature.empty()){
            m_dispatch_thread = std::thread(&ProController_SysbotBase::thread_body, this);
            return;
        }

    }while (false);

    m_error_string = html_color_text("Missing Feature: " + missing_feature, COLOR_RED);

}
ProController_SysbotBase::~ProController_SysbotBase(){
    stop();
    if (m_dispatch_thread.joinable()){
        m_dispatch_thread.join();
    }
}
void ProController_SysbotBase::stop(){
    if (m_stopping.exchange(true)){
        return;
    }
    ProController::stop();
    {
        std::lock_guard<std::mutex> lg(m_state_lock);
        m_cv.notify_all();
    }
}



void ProController_SysbotBase::cancel_all_commands(){
//    cout << "ProController_SysbotBase::cancel_all_commands()" << endl;
    std::lock_guard<std::mutex> lg(m_state_lock);
    size_t queue_size = m_command_queue.size();
    if (queue_size > 0){
        m_command_queue.clear();
        m_is_active = false;
        m_cv.notify_all();
    }
    this->clear_on_next();
    m_logger.log("cancel_all_commands(): Command Queue Size = " + std::to_string(queue_size), COLOR_DARKGREEN);
}
void ProController_SysbotBase::replace_on_next_command(){
//    cout << "ProController_SysbotBase::replace_on_next_command - Enter()" << endl;
    std::lock_guard<std::mutex> lg(m_state_lock);
    m_cv.notify_all();
    m_replace_on_next = true;
    this->clear_on_next();
    m_logger.log("replace_on_next_command(): Command Queue Size = " + std::to_string(m_command_queue.size()), COLOR_DARKGREEN);
}


void ProController_SysbotBase::wait_for_all(const Cancellable* cancellable){
//    cout << "ProController_SysbotBase::wait_for_all - Enter()" << endl;
    std::lock_guard<std::mutex> lg0(m_issue_lock);
    std::unique_lock<std::mutex> lg1(m_state_lock);
    m_logger.log("wait_for_all(): Command Queue Size = " + std::to_string(m_command_queue.size()), COLOR_DARKGREEN);
    this->issue_wait_for_all(cancellable);
    m_cv.wait(lg1, [this]{
        return m_command_queue.empty() || m_replace_on_next;
    });
    if (cancellable){
        cancellable->throw_if_cancelled();
    }
//    cout << "ProController_SysbotBase::wait_for_all - Exit()" << endl;
}
void ProController_SysbotBase::push_state(const Cancellable* cancellable, WallDuration duration){
    //  Must be called inside "m_state_lock".

    if (cancellable){
        cancellable->throw_if_cancelled();
    }
    if (!is_ready()){
        throw InvalidConnectionStateException();
    }

    Button buttons = BUTTON_NONE;
    for (size_t c = 0; c < 14; c++){
        buttons |= m_buttons[c].is_busy()
            ? (Button)((uint16_t)1 << c)
            : BUTTON_NONE;
    }

    DpadPosition dpad = m_dpad.is_busy() ? m_dpad.position : DPAD_NONE;

    uint8_t left_x = 128;
    uint8_t left_y = 128;
    uint8_t right_x = 128;
    uint8_t right_y = 128;
    if (m_left_joystick.is_busy()){
        left_x = m_left_joystick.x;
        left_y = m_left_joystick.y;
    }
    if (m_right_joystick.is_busy()){
        right_x = m_right_joystick.x;
        right_y = m_right_joystick.y;
    }

    std::unique_lock<std::mutex> lg(m_state_lock, std::adopt_lock_t());

    m_cv.wait(lg, [this]{
        return m_command_queue.size() < QUEUE_SIZE || m_replace_on_next;
    });

    lg.release();

    if (cancellable){
        cancellable->throw_if_cancelled();
    }

    if (m_replace_on_next){
        m_command_queue.clear();
        m_is_active = false;
        m_replace_on_next = false;
    }

    if (m_command_queue.empty()){
        m_cv.notify_all();
    }

    Command& command = m_command_queue.push_back();

    command.state.buttons = buttons;
    command.state.dpad = dpad;
    command.state.left_x = left_x;
    command.state.left_y = left_y;
    command.state.right_x = right_x;
    command.state.right_y = right_y;

    command.duration = std::chrono::duration_cast<Milliseconds>(duration);
}


void ProController_SysbotBase::send_botbase_request(
    const Cancellable* cancellable,
    const BotBaseRequest& request
){
    throw InternalProgramError(
        &m_logger, PA_CURRENT_FUNCTION,
        "ProController_SysbotBase::send_botbase_request(): Not supported."
    );
}
BotBaseMessage ProController_SysbotBase::send_botbase_request_and_wait(
    const Cancellable* cancellable,
    const BotBaseRequest& request
){
    throw InternalProgramError(
        &m_logger, PA_CURRENT_FUNCTION,
        "ProController_SysbotBase::send_botbase_request_and_wait(): Not supported."
    );
}



void ProController_SysbotBase::send_diff(
    const ProControllerState& old_state,
    const ProControllerState& new_state
){
#if 0
    m_logger.log(
        "send_diff(): (" + button_to_string(new_state.buttons) +
        "), dpad(" + dpad_to_string(new_state.dpad) +
        "), LJ(" + std::to_string(new_state.left_x) + "," + std::to_string(new_state.left_y) +
        "), RJ(" + std::to_string(new_state.right_x) + "," + std::to_string(new_state.right_y) +
        ")",
        COLOR_DARKGREEN
    );
#endif

    //  These need to match:
    //  https://github.com/olliz0r/sys-botbase/blob/master/sys-botbase/source/util.c#L145
    static const std::vector<std::pair<Button, std::string>> BUTTON_MAP{
        {BUTTON_Y, "Y"},
        {BUTTON_B, "B"},
        {BUTTON_A, "A"},
        {BUTTON_X, "X"},
        {BUTTON_L, "L"},
        {BUTTON_R, "R"},
        {BUTTON_ZL, "ZL"},
        {BUTTON_ZR, "ZR"},
        {BUTTON_MINUS, "MINUS"},
        {BUTTON_PLUS, "PLUS"},
        {BUTTON_LCLICK, "LSTICK"},
        {BUTTON_RCLICK, "RSTICK"},
        {BUTTON_HOME, "HOME"},
        {BUTTON_CAPTURE, "CAPTURE"},
        {BUTTON_UP, "DU"},
        {BUTTON_RIGHT, "DR"},
        {BUTTON_DOWN, "DD"},
        {BUTTON_LEFT, "DL"},
    };

    std::string message;


    //  Merge the dpad states.
    ButtonFlagType old_buttons = old_state.buttons;
    ButtonFlagType new_buttons = new_state.buttons;

    SplitDpad old_dpad = convert_unified_to_split_dpad(old_state.dpad);
    if (old_dpad.up)    old_buttons |= BUTTON_UP;
    if (old_dpad.right) old_buttons |= BUTTON_RIGHT;
    if (old_dpad.down)  old_buttons |= BUTTON_DOWN;
    if (old_dpad.left)  old_buttons |= BUTTON_LEFT;

    SplitDpad new_dpad = convert_unified_to_split_dpad(new_state.dpad);
    if (new_dpad.up)    new_buttons |= BUTTON_UP;
    if (new_dpad.right) new_buttons |= BUTTON_RIGHT;
    if (new_dpad.down)  new_buttons |= BUTTON_DOWN;
    if (new_dpad.left)  new_buttons |= BUTTON_LEFT;


    if (old_buttons != new_buttons){
        for (const auto& button : BUTTON_MAP){
            ButtonFlagType mask = (ButtonFlagType)button.first;
            bool before = (ButtonFlagType)old_buttons & mask;
            bool after = (ButtonFlagType)new_buttons & mask;
            if (before == after){
                continue;
            }
            if (after){
                message += "press " + button.second + "\n";
            }else{
                message += "release " + button.second + "\n";
            }
        }
    }

    if (old_state.left_x != new_state.left_x ||
        old_state.left_y != new_state.left_y
    ){
        message += "setStick LEFT ";
        message += std::to_string(((uint16_t)new_state.left_x - 128) << 8);
        message += " ";
        message += std::to_string(((uint16_t)128 - new_state.left_y) << 8);
        message += "\n";
    }
    if (old_state.right_x != new_state.right_x ||
        old_state.right_y != new_state.right_y
    ){
        message += "setStick RIGHT ";
        message += std::to_string(((uint16_t)new_state.right_x - 128) << 8);
        message += " ";
        message += std::to_string(((uint16_t)128 - new_state.right_y) << 8);
        message += "\n";
    }

    if (message.empty()){
        return;
    }

//    cout << message << endl;
    m_connection.write_data(message);

    if (GlobalSettings::instance().LOG_EVERYTHING){
        m_logger.log("sys-botbase: " + message);
    }
}


void ProController_SysbotBase::thread_body(){
    GlobalSettings::instance().PERFORMANCE->REALTIME_THREAD_PRIORITY.set_on_this_thread();
    std::chrono::microseconds EARLY_WAKE = GlobalSettings::instance().PERFORMANCE->PRECISE_WAKE_MARGIN;

    ProControllerState current_state;

    std::unique_lock<std::mutex> lg(m_state_lock);
    while (!m_stopping.load(std::memory_order_relaxed)){
        if (m_command_queue.empty()){
            m_is_active = false;
            if (current_state.is_neutral()){
                m_cv.wait(lg);
                continue;
            }

            send_diff(current_state, ProControllerState());
            current_state.clear();
            continue;
        }

        WallClock now = current_time();

        //  Check the next item in the schedule.
        Command& command = m_command_queue.front();

        //  Waking up from idle.
        if (!m_is_active){
            m_is_active = true;
            m_queue_start_time = now;

            send_diff(current_state, command.state);
            current_state = command.state;

            WallClock expiration = m_queue_start_time + command.duration;
            m_cv.wait_until(lg, expiration - EARLY_WAKE);
            pause();
            continue;
        }

        //  Already running.
        WallClock expiration = m_queue_start_time + command.duration;

        //  Current command hasn't expired yet.
        if (now < expiration){
            m_cv.wait_until(lg, expiration - EARLY_WAKE);
            pause();
            continue;
        }

        //  Command has expired. We can pop it.
        m_is_active = false;
        m_queue_start_time = expiration;
        m_command_queue.pop_front();
        m_cv.notify_all();

        //  Check how much we shot passed the expiration time.
//        WallDuration delay = now - expiration;

//        cout << "Delay = " << std::chrono::duration_cast<std::chrono::microseconds>(delay) << endl;
    }

    ProControllerState neutral_state;
    send_diff(current_state, neutral_state);
}























}
}
