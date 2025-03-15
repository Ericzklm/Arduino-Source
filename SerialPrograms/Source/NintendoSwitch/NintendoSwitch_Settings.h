/*  Nintendo Switch Settings
 *
 *  From: https://github.com/PokemonAutomation/
 *
 */

#ifndef PokemonAutomation_NintendoSwitch_Settings_H
#define PokemonAutomation_NintendoSwitch_Settings_H

#include "Common/Cpp/ImageResolution.h"
#include "Common/Cpp/Options/StaticTextOption.h"
#include "Common/Cpp/Options/BooleanCheckBoxOption.h"
#include "Common/Cpp/Options/TimeDurationOption.h"
#include "CommonFramework/Panels/SettingsPanel.h"
#include "Options/NintendoSwitch_CodeEntrySettingsOption.h"
#include "Controllers/NintendoSwitch_ControllerSettings.h"
#include "Controllers/NintendoSwitch_KeyboardMapping.h"

namespace PokemonAutomation{
namespace NintendoSwitch{


extern const Resolution DEFAULT_RESOLUTION;



class ConsoleSettings : public BatchOption{
    ConsoleSettings();
    virtual void load_json(const JsonValue& json) override;
public:
    static ConsoleSettings& instance();

    ControllerSettingsTable CONTROLLER_SETTINGS;

    MillisecondsOption SETTINGS_TO_HOME_DELAY0;
    BooleanCheckBoxOption START_GAME_REQUIRES_INTERNET;
    MillisecondsOption START_GAME_INTERNET_CHECK_DELAY0;
    BooleanCheckBoxOption TOLERATE_SYSTEM_UPDATE_MENU_FAST;
    BooleanCheckBoxOption TOLERATE_SYSTEM_UPDATE_MENU_SLOW;

    DigitEntryTimingsOption DIGIT_ENTRY;
    KeyboardEntryTimingsOption KEYBOARD_ENTRY;

    SectionDividerOption KEYBOARD_SECTION;
    KeyboardMappingOption KEYBOARD_MAPPINGS;

private:
    bool m_loaded;
};





class ConsoleSettings_Descriptor : public PanelDescriptor{
public:
    ConsoleSettings_Descriptor();
};


class ConsoleSettingsPanel : public SettingsPanelInstance{
public:
    ConsoleSettingsPanel(const ConsoleSettings_Descriptor& descriptor);
private:
    ConsoleSettings& settings;
};



}
}
#endif
