/*  SerialPABotBase: Wireless Pro Controller
 *
 *  From: https://github.com/PokemonAutomation/
 *
 */

#include "Common/SerialPABotBase/SerialPABotBase_Messages_ESP32.h"
#include "ClientSource/Libraries/MessageConverter.h"
#include "NintendoSwitch_SerialPABotBase_WirelessProController.h"

//#include <iostream>
//using std::cout;
//using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{



SerialPABotBase_WirelessProController::SerialPABotBase_WirelessProController(
    Logger& logger,
    SerialPABotBase::SerialPABotBase_Connection& connection
)
    : SerialPABotBase_WirelessController(
        logger,
        connection,
        ControllerType::NintendoSwitch_WirelessProController
    )
{}
SerialPABotBase_WirelessProController::~SerialPABotBase_WirelessProController(){
    ProController::stop();
    SerialPABotBase_WirelessController::stop();
}


void SerialPABotBase_WirelessProController::push_state(const Cancellable* cancellable, WallDuration duration){
    //  https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/bluetooth_hid_notes.md

    PABB_ESP32_NintendoSwitch_ButtonState report{
        .button3 = 0,
        .button4 = 0,
        .button5 = 0,
        .left_joystick = {0x00, 0x08, 0x80},
        .right_joystick = {0x00, 0x08, 0x80},
        .vibrator = 0x00,
    };

    Button all_buttons = BUTTON_NONE;
    for (size_t c = 0; c < TOTAL_BUTTONS; c++){
        if (!m_buttons[c].is_busy()){
            continue;
        }
        Button button = (Button)((ButtonFlagType)1 << c);
        all_buttons |= button;
        switch (button){
        //  Right
        case BUTTON_Y:          report.button3 |= 1 << 0; break;
        case BUTTON_X:          report.button3 |= 1 << 1; break;
        case BUTTON_B:          report.button3 |= 1 << 2; break;
        case BUTTON_A:          report.button3 |= 1 << 3; break;
//        case BUTTON_RIGHT_SR:   report.button3 |= 1 << 4; break;
//        case BUTTON_RIGHT_SL:   report.button3 |= 1 << 5; break;
        case BUTTON_R:          report.button3 |= 1 << 6; break;
        case BUTTON_ZR:         report.button3 |= 1 << 7; break;

        //  Shared
        case BUTTON_MINUS:      report.button4 |= 1 << 0; break;
        case BUTTON_PLUS:       report.button4 |= 1 << 1; break;
        case BUTTON_RCLICK:     report.button4 |= 1 << 2; break;
        case BUTTON_LCLICK:     report.button4 |= 1 << 3; break;
        case BUTTON_HOME:       report.button4 |= 1 << 4; break;
        case BUTTON_CAPTURE:    report.button4 |= 1 << 5; break;

        //  Left
        case BUTTON_DOWN:       report.button5 |= 1 << 0; break;
        case BUTTON_UP:         report.button5 |= 1 << 1; break;
        case BUTTON_RIGHT:      report.button5 |= 1 << 2; break;
        case BUTTON_LEFT:       report.button5 |= 1 << 3; break;
//        case BUTTON_LEFT_SR:    report.button5 |= 1 << 4; break;
//        case BUTTON_LEFT_SL:    report.button5 |= 1 << 5; break;
        case BUTTON_L:          report.button5 |= 1 << 6; break;
        case BUTTON_ZL:         report.button5 |= 1 << 7; break;

        default:;
        }
    }

    if (m_dpad.is_busy()){
        SplitDpad dpad = convert_unified_to_split_dpad(m_dpad.position);
        report.button5 |= (dpad.down  ? 1 : 0) << 0;
        report.button5 |= (dpad.up    ? 1 : 0) << 1;
        report.button5 |= (dpad.right ? 1 : 0) << 2;
        report.button5 |= (dpad.left  ? 1 : 0) << 3;
    }

    //  Left Stick
    if (m_left_joystick.is_busy()){
        encode_joystick<JOYSTICK_MIN_THRESHOLD, JOYSTICK_MAX_THRESHOLD>(
            report.left_joystick,
            m_left_joystick.x, m_left_joystick.y
        );
    }

    //  Right Stick
    if (m_right_joystick.is_busy()){
        encode_joystick<JOYSTICK_MIN_THRESHOLD, JOYSTICK_MAX_THRESHOLD>(
            report.right_joystick,
            m_right_joystick.x, m_right_joystick.y
        );
    }

    issue_report(cancellable, report, duration);

#if 0
    m_logger.log(
        "push_state(): (" + button_to_string(all_buttons) +
        "), dpad(" + dpad_to_string(m_dpad.position) +
        "), LJ(" + std::to_string(m_left_joystick.x) + "," + std::to_string(m_left_joystick.y) +
        "), RJ(" + std::to_string(m_right_joystick.x) + "," + std::to_string(m_right_joystick.y) +
        "), hold = " + std::to_string(std::chrono::duration_cast<Milliseconds>(duration).count()) + "ms",
        COLOR_DARKGREEN
    );
#endif
}





}
}
