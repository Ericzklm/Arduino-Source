/*  Nintendo Switch Controller Buttons
 *
 *  From: https://github.com/PokemonAutomation/
 *
 */

#include "NintendoSwitch_ControllerButtons.h"
#include "Common/Cpp/EnumStringMap.h"
#include "Common/Cpp/StringTools.h"
#include "NintendoSwitch_ControllerState.h"

namespace PokemonAutomation{
namespace NintendoSwitch{

const EnumStringMap<Button> BUTTON_STRINGS{
    {Button::BUTTON_NONE,              "none"},
    {Button::BUTTON_Y,                 "Y"},
    {Button::BUTTON_B,                 "B"},
    {Button::BUTTON_A,                 "A"},
    {Button::BUTTON_X,                 "X"},
    {Button::BUTTON_L,                 "L"},
    {Button::BUTTON_R,                 "R"},
    {Button::BUTTON_ZL,                "ZL"},
    {Button::BUTTON_ZR,                "ZR"},
    {Button::BUTTON_MINUS,             "-"},
    {Button::BUTTON_PLUS,              "+"},
    {Button::BUTTON_LCLICK,            "LJ"},
    {Button::BUTTON_RCLICK,            "RJ"},
    {Button::BUTTON_HOME,              "HOME"},
    {Button::BUTTON_CAPTURE,           "CAPTURE"},
    {Button::BUTTON_GR,                "GR"},
    {Button::BUTTON_GL,                "GL"},
    {Button::BUTTON_UP,                "Up"},
    {Button::BUTTON_RIGHT,             "Right"},
    {Button::BUTTON_DOWN,              "Down"},
    {Button::BUTTON_LEFT,              "Left"},
    {Button::BUTTON_LEFT_SL,           "Left-SL"},
    {Button::BUTTON_LEFT_SR,           "Left-SR"},
    {Button::BUTTON_RIGHT_SL,          "Right-SL"},
    {Button::BUTTON_RIGHT_SR,          "Right-SR"},
    {Button::BUTTON_C,                 "C"},
};    

const EnumStringMap<Button> BUTTON_CODE_STRINGS{
    {Button::BUTTON_NONE,              "BUTTON_NONE"},
    {Button::BUTTON_Y,                 "BUTTON_Y"},
    {Button::BUTTON_B,                 "BUTTON_B"},
    {Button::BUTTON_A,                 "BUTTON_A"},
    {Button::BUTTON_X,                 "BUTTON_X"},
    {Button::BUTTON_L,                 "BUTTON_L"},
    {Button::BUTTON_R,                 "BUTTON_R"},
    {Button::BUTTON_ZL,                "BUTTON_ZL"},
    {Button::BUTTON_ZR,                "BUTTON_ZR"},
    {Button::BUTTON_MINUS,             "BUTTON_MINUS"},
    {Button::BUTTON_PLUS,              "BUTTON_PLUS"},
    {Button::BUTTON_LCLICK,            "BUTTON_LCLICK"},
    {Button::BUTTON_RCLICK,            "BUTTON_RCLICK"},
    {Button::BUTTON_HOME,              "BUTTON_HOME"},
    {Button::BUTTON_CAPTURE,           "BUTTON_CAPTURE"},
    {Button::BUTTON_GR,                "BUTTON_GR"},
    {Button::BUTTON_GL,                "BUTTON_GL"},
    {Button::BUTTON_UP,                "BUTTON_UP"},
    {Button::BUTTON_RIGHT,             "BUTTON_RIGHT"},
    {Button::BUTTON_DOWN,              "BUTTON_DOWN"},
    {Button::BUTTON_LEFT,              "BUTTON_LEFT"},
    {Button::BUTTON_LEFT_SL,           "BUTTON_LEFT_SL"},
    {Button::BUTTON_LEFT_SR,           "BUTTON_LEFT_SR"},
    {Button::BUTTON_RIGHT_SL,          "BUTTON_RIGHT_SL"},
    {Button::BUTTON_RIGHT_SR,          "BUTTON_RIGHT_SR"},
    {Button::BUTTON_C,                 "BUTTON_C"},
};  


std::string button_to_string(Button button){
    std::string str;
    if (button & BUTTON_Y)          str += BUTTON_STRINGS.get_string(BUTTON_Y) + " ";
    if (button & BUTTON_B)          str += BUTTON_STRINGS.get_string(BUTTON_B) + " ";
    if (button & BUTTON_A)          str += BUTTON_STRINGS.get_string(BUTTON_A) + " ";
    if (button & BUTTON_X)          str += BUTTON_STRINGS.get_string(BUTTON_X) + " ";
    if (button & BUTTON_L)          str += BUTTON_STRINGS.get_string(BUTTON_L) + " ";
    if (button & BUTTON_R)          str += BUTTON_STRINGS.get_string(BUTTON_R) + " ";
    if (button & BUTTON_ZL)         str += BUTTON_STRINGS.get_string(BUTTON_ZL) + " ";
    if (button & BUTTON_ZR)         str += BUTTON_STRINGS.get_string(BUTTON_ZR) + " ";
    if (button & BUTTON_MINUS)      str += BUTTON_STRINGS.get_string(BUTTON_MINUS) + " ";
    if (button & BUTTON_PLUS)       str += BUTTON_STRINGS.get_string(BUTTON_PLUS) + " ";
    if (button & BUTTON_LCLICK)     str += BUTTON_STRINGS.get_string(BUTTON_LCLICK) + " ";
    if (button & BUTTON_RCLICK)     str += BUTTON_STRINGS.get_string(BUTTON_RCLICK) + " ";
    if (button & BUTTON_HOME)       str += BUTTON_STRINGS.get_string(BUTTON_HOME) + " ";
    if (button & BUTTON_CAPTURE)    str += BUTTON_STRINGS.get_string(BUTTON_CAPTURE) + " ";
    if (button & BUTTON_GR)         str += BUTTON_STRINGS.get_string(BUTTON_GR) + " ";
    if (button & BUTTON_GL)         str += BUTTON_STRINGS.get_string(BUTTON_GL) + " ";
    if (button & BUTTON_UP)         str += BUTTON_STRINGS.get_string(BUTTON_UP) + " ";
    if (button & BUTTON_RIGHT)      str += BUTTON_STRINGS.get_string(BUTTON_RIGHT) + " ";
    if (button & BUTTON_DOWN)       str += BUTTON_STRINGS.get_string(BUTTON_DOWN) + " ";
    if (button & BUTTON_LEFT)       str += BUTTON_STRINGS.get_string(BUTTON_LEFT) + " ";
    if (button & BUTTON_LEFT_SL)    str += BUTTON_STRINGS.get_string(BUTTON_LEFT_SL) + " ";
    if (button & BUTTON_LEFT_SR)    str += BUTTON_STRINGS.get_string(BUTTON_LEFT_SR) + " ";
    if (button & BUTTON_RIGHT_SL)   str += BUTTON_STRINGS.get_string(BUTTON_RIGHT_SL) + " ";
    if (button & BUTTON_RIGHT_SR)   str += BUTTON_STRINGS.get_string(BUTTON_RIGHT_SR) + " ";
    if (button & BUTTON_C)          str += BUTTON_STRINGS.get_string(BUTTON_C) + " ";
    if (str.empty()){
        str = BUTTON_STRINGS.get_string(BUTTON_NONE);
    }
    if (str.back() == ' '){
        str.pop_back();
    }
    return str;
}

Button string_to_button(std::string multi_button_string){
    std::vector<std::string> string_vector = StringTools::split(multi_button_string, " ");
    Button button_result = BUTTON_NONE;
    for (const std::string& button_string : string_vector){
        Button one_button = BUTTON_STRINGS.get_enum(button_string);
        button_result |= one_button;
    }

    return button_result;
}

std::string button_to_code_string(Button button){
    std::string str;
    if (button & BUTTON_Y)          str += BUTTON_CODE_STRINGS.get_string(BUTTON_Y) + " | ";
    if (button & BUTTON_B)          str += BUTTON_CODE_STRINGS.get_string(BUTTON_B) + " | ";
    if (button & BUTTON_A)          str += BUTTON_CODE_STRINGS.get_string(BUTTON_A) + " | ";
    if (button & BUTTON_X)          str += BUTTON_CODE_STRINGS.get_string(BUTTON_X) + " | ";
    if (button & BUTTON_L)          str += BUTTON_CODE_STRINGS.get_string(BUTTON_L) + " | ";
    if (button & BUTTON_R)          str += BUTTON_CODE_STRINGS.get_string(BUTTON_R) + " | ";
    if (button & BUTTON_ZL)         str += BUTTON_CODE_STRINGS.get_string(BUTTON_ZL) + " | ";
    if (button & BUTTON_ZR)         str += BUTTON_CODE_STRINGS.get_string(BUTTON_ZR) + " | ";
    if (button & BUTTON_MINUS)      str += BUTTON_CODE_STRINGS.get_string(BUTTON_MINUS) + " | ";
    if (button & BUTTON_PLUS)       str += BUTTON_CODE_STRINGS.get_string(BUTTON_PLUS) + " | ";
    if (button & BUTTON_LCLICK)     str += BUTTON_CODE_STRINGS.get_string(BUTTON_LCLICK) + " | ";
    if (button & BUTTON_RCLICK)     str += BUTTON_CODE_STRINGS.get_string(BUTTON_RCLICK) + " | ";
    if (button & BUTTON_HOME)       str += BUTTON_CODE_STRINGS.get_string(BUTTON_HOME) + " | ";
    if (button & BUTTON_CAPTURE)    str += BUTTON_CODE_STRINGS.get_string(BUTTON_CAPTURE) + " | ";
    if (button & BUTTON_GR)         str += BUTTON_CODE_STRINGS.get_string(BUTTON_GR) + " | ";
    if (button & BUTTON_GL)         str += BUTTON_CODE_STRINGS.get_string(BUTTON_GL) + " | ";
    if (button & BUTTON_UP)         str += BUTTON_CODE_STRINGS.get_string(BUTTON_UP) + " | ";
    if (button & BUTTON_RIGHT)      str += BUTTON_CODE_STRINGS.get_string(BUTTON_RIGHT) + " | ";
    if (button & BUTTON_DOWN)       str += BUTTON_CODE_STRINGS.get_string(BUTTON_DOWN) + " | ";
    if (button & BUTTON_LEFT)       str += BUTTON_CODE_STRINGS.get_string(BUTTON_LEFT) + " | ";
    if (button & BUTTON_LEFT_SL)    str += BUTTON_CODE_STRINGS.get_string(BUTTON_LEFT_SL) + " | ";
    if (button & BUTTON_LEFT_SR)    str += BUTTON_CODE_STRINGS.get_string(BUTTON_LEFT_SR) + " | ";
    if (button & BUTTON_RIGHT_SL)   str += BUTTON_CODE_STRINGS.get_string(BUTTON_RIGHT_SL) + " | ";
    if (button & BUTTON_RIGHT_SR)   str += BUTTON_CODE_STRINGS.get_string(BUTTON_RIGHT_SR) + " | ";
    if (button & BUTTON_C)          str += BUTTON_CODE_STRINGS.get_string(BUTTON_C) + " | ";
    if (str.empty()){
        str = BUTTON_CODE_STRINGS.get_string(BUTTON_NONE);
    }
    if (str.substr(str.size() - 3) == " | "){
        str = str.substr(0, str.size() - 3);
    }
    return str;
}

const EnumStringMap<DpadPosition> DPAD_STRINGS{
    {DpadPosition::DPAD_UP,              "up"},
    {DpadPosition::DPAD_UP_RIGHT,        "up-right"},
    {DpadPosition::DPAD_RIGHT,           "right"},
    {DpadPosition::DPAD_DOWN_RIGHT,      "down-right"},
    {DpadPosition::DPAD_DOWN,            "down"},
    {DpadPosition::DPAD_DOWN_LEFT,       "down-left"},
    {DpadPosition::DPAD_LEFT,            "left"},
    {DpadPosition::DPAD_UP_LEFT,         "up-left"},
    {DpadPosition::DPAD_NONE,            "none"},
}; 

const EnumStringMap<DpadPosition> DPAD_CODE_STRINGS{
    {DpadPosition::DPAD_UP,              "DPAD_UP"},
    {DpadPosition::DPAD_UP_RIGHT,        "DPAD_UP_RIGHT"},
    {DpadPosition::DPAD_RIGHT,           "DPAD_RIGHT"},
    {DpadPosition::DPAD_DOWN_RIGHT,      "DPAD_DOWN_RIGHT"},
    {DpadPosition::DPAD_DOWN,            "DPAD_DOWN"},
    {DpadPosition::DPAD_DOWN_LEFT,       "DPAD_DOWN_LEFT"},
    {DpadPosition::DPAD_LEFT,            "DPAD_LEFT"},
    {DpadPosition::DPAD_UP_LEFT,         "DPAD_UP_LEFT"},
    {DpadPosition::DPAD_NONE,            "DPAD_NONE"},
}; 


std::string dpad_to_string(DpadPosition dpad){
    try {
        return DPAD_STRINGS.get_string(dpad);
    }catch (ParseException&){
        return "unknown";
    }
}

std::string dpad_to_code_string(DpadPosition dpad){
    return DPAD_CODE_STRINGS.get_string(dpad);
}

DpadPosition string_to_dpad(std::string dpad_string){
    return DPAD_STRINGS.get_enum(dpad_string);
}



}
}
