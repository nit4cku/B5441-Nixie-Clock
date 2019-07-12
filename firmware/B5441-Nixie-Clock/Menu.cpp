/*
 * Copyright (c) 2018 nitacku
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * @file        Menu.cpp
 * @summary     Digital Clock for B5441 Nixie display tubes
 * @version     3.2
 * @author      nitacku
 * @data        14 August 2018
 */

#include "Menu.h"

extern StateStruct g_state;         // struct
extern Config g_config;             // struct
extern CDS3232 g_rtc;               // class
extern CDisplay g_display;          // class
extern CAudio g_audio;              // class
extern uint8_t g_song_entries;      // integral
extern bool IsInputIncrement(void); // Function
extern bool IsInputSelect(void);    // Function
extern bool IsInputUpdate(void);    // Function

// Preferred character conversion
// A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
// 4 8 6 2 3 4 6 8 1 3 8 1 0 0 0 9 2 2 5 7 0 0 3 8 9 2
// _ _     _   _   _     _     _ _ _ _ _ _         _ _

void MenuInfo(void)
{
    char s[DISPLAY_COUNT + 1];
    uint8_t function = 0;
    uint32_t timeout = Timeout::INFO;

    while (IsInputSelect() && --timeout);

    if (timeout)
    {
        do
        {
            timeout = Timeout::INFO;
            while (!IsInputSelect() && --timeout);
            
            if (!timeout)
            {
                break;
            }
            
            switch (function)
            {
            case 0:
            {
                float f_value;
                int i_fraction;
                int i_value;

                f_value = g_rtc.GetTemperature(); // Get temperature in Celsius

                if (g_config.temperature_unit == CRTC::Unit::F)
                {
                    f_value = g_rtc.ConvertTemperature(g_rtc.GetTemperature(), CRTC::Unit::C, CRTC::Unit::F);
                }
                
                i_value = f_value;
                i_fraction = ((f_value - i_value) * 100.0);

                snprintf(s, DISPLAY_COUNT + 1, " %03u;%02u ", i_value, i_fraction);
                g_display.EffectScroll(s, CDisplay::Direction::LEFT, 80);
                break;
            }
            case 1:
                snprintf(s, DISPLAY_COUNT+1, "  %04u  ", VERSION);
                g_display.SetDisplayValue(s);
                g_display.EffectSlotMachine(35);
                break;
            case 2:
                RestoreOutOfBox();
                break;
            }

            function++;
            timeout = Timeout::INFO;
            while (IsInputSelect() && --timeout); // Wait until release
            
            if (!timeout)
            {
                Detonate();
            }
        }
        while ((function < 3) && timeout);
    }
    else
    {
        DisplayState(State::DISABLE);
    }
}


void MenuSettings(void)
{
    // Use full brightness for Menu
    VoltageState(State::ENABLE);
    g_display.SetDisplayBrightness(CDisplay::Brightness::MAX);
    
    CDisplay::PromptSelectStruct prompt_select;
    prompt_select.item_count = MENU_ITEM_COUNT;
    prompt_select.initial_selection = MENU_ITEM_DIVERGENCE;
    prompt_select.display_mode = CDisplay::Mode::SCROLL;
    prompt_select.title = nullptr;

    // Copy item table to local variable
    char* array[sizeof(menu_item_array) / sizeof(char*)];
    memcpy_P(array, menu_item_array, sizeof(array));
    prompt_select.item_array = reinterpret_cast<type_const_char_ptr*>(array);
    int8_t selection = g_display.PromptSelect(prompt_select, Timeout::MENU);
    
    if (selection > -1)
    {
        switch (selection)
        {
        default:
        case MENU_ITEM_DIVERGENCE:
            DivergenceMeter();
            break;
        
        case MENU_ITEM_ALARM:
        {
            uint8_t alarm = 0; // Track selection
            
            if (SetAlarmState(alarm))
            {
                if (SetAlarmTime(alarm))
                {
                    if (SetAlarmDays(alarm))
                    {
                        SetMusic(g_config.alarm[alarm].music);
                    }
                }
            }

            break;
        }
            
        case MENU_ITEM_BRIGHTNESS:
            if (SetBrightness() && (g_config.brightness == CDisplay::Brightness::AUTO))
            {
                if (SetGain())
                {
                    SetOffset();
                }
            }
            break;

        case MENU_ITEM_CONFIG:
            if (SetTimeFormat())
            {
                if (SetDateFormat())
                {
                    if (SetTemperatureUnit())
                    {
                        SetBlip();
                    }
                }
            }
            break;

        case MENU_ITEM_BLANK:
            SetBlank();
            break;
            
        case MENU_ITEM_TIME:
            SetTime();
            break;

        case MENU_ITEM_DATE:
            SetDate();
            break;
            
        case MENU_ITEM_MUSIC:
            SetMusic(g_config.music_timer);
            break;

        case MENU_ITEM_TIMER:
            SetTimer();
            break;
        }
    }

    g_display.SetDisplayBrightness(g_config.brightness);
}


int8_t SelectCycle(const Cycle init_value)
{
    CDisplay::PromptSelectStruct prompt_select;
    prompt_select.item_count = 2;
    prompt_select.initial_selection = getValue(init_value);
    prompt_select.title = F(" 932102 "); // PERIOD
    type_const_char_ptr item_array[] = {F("   40   "), F("   90   ")}; // AM PM
    prompt_select.item_array = item_array;
    return g_display.PromptSelect(prompt_select, Timeout::SELECT);
}


int8_t SelectState(CDisplay::PromptSelectStruct& prompt_select)
{
    prompt_select.item_count = 2;
    prompt_select.display_mode = CDisplay::Mode::STATIC;
    type_const_char_ptr item_array[] = {F("2154813 "), F(" 304813 ")}; // ENABLE DISABLE
    prompt_select.item_array = item_array;
    return g_display.PromptSelect(prompt_select, Timeout::SELECT);
}


bool SelectRTCValue(CDisplay::PromptValueStruct& prompt_value)
{
    Cycle current_cycle = (prompt_value.item_value[0] < 12) ? Cycle::AM : Cycle::PM;

    if (g_config.time_format == FormatTime::H24)
    {
        prompt_value.item_lower_limit = (const type_const_uint8 []){0, 0, 0};
        prompt_value.item_upper_limit = (const type_const_uint8 []){23, 59, 59};
    }
    else
    {
        prompt_value.item_value[0] = FormatHour(prompt_value.item_value[0]);
        prompt_value.item_lower_limit = (const type_const_uint8 []){1, 0, 0};
        prompt_value.item_upper_limit = (const type_const_uint8 []){12, 59, 59};
    }

    if (g_display.PromptValue(prompt_value, Timeout::VALUE) > -1)
    {
        if (g_config.time_format == FormatTime::H12)
        {
            int8_t result = SelectCycle(current_cycle);

            if (result == -1)
            {
                return false;
            }

            prompt_value.item_value[0] %= 12;
            prompt_value.item_value[0] += (static_cast<Cycle>(result) == Cycle::PM) ? 12 : 0;
        }

        return true;
    }

    return false;
}


bool RestoreOutOfBox(void)
{
    CDisplay::PromptSelectStruct prompt_select;
    prompt_select.item_count = 2;
    prompt_select.title = F(" 23537  "); // RESET
    type_const_char_ptr item_array[] = {F(" 640631 "), F(" 23527  ")}; // CANCEL RESET
    prompt_select.item_array = item_array;
    int8_t selection = g_display.PromptSelect(prompt_select, Timeout::SELECT);

    if (selection > 0)
    {
        Config new_config; // Use default constructor values
        SetConfig(new_config); // Write to EEPROM
        GetConfig(g_config); // Read from EEPROM
        return true;
    }

    return false;
}


bool SetBlank(void)
{
    char s[DISPLAY_COUNT + 1];
    uint8_t hour, minute;
    CDisplay::PromptValueStruct prompt_value;

    for (uint8_t index = 0; index < 2; index++)
    {
        if (index)
        {
            prompt_value.title = F("9032 00 "); // POWR ON
            hour = g_config.blank_end / 3600;
            minute = (g_config.blank_end / 60) % 60;
        }
        else
        {
            prompt_value.title = F("9032 044"); // POWR OFF
            hour = g_config.blank_begin / 3600;
            minute = (g_config.blank_begin / 60) % 60;
        }

        snprintf_P(s, DISPLAY_COUNT + 1, PSTR(" %02u;%02u %01u"), FormatHour(hour), minute, index);
        type_const_uint8 item_value[] = {hour, minute};
        prompt_value.item_count = 2;
        prompt_value.item_position = (const uint8_t []){1, 4};
        prompt_value.item_digit_count = (const uint8_t []){2, 2};
        prompt_value.item_value = item_value;
        prompt_value.initial_display = s;

        if (SelectRTCValue(prompt_value))
        {
            uint32_t value = GetSeconds(prompt_value.item_value[0], prompt_value.item_value[1], 0);
            
            if (index)
            {
                g_config.blank_end = value;
            }
            else
            {
                g_config.blank_begin = value;
            }

            SetConfig(g_config);
        }
        else
        {
            return false;
        }
    }

    return true;
}


bool SetBrightness(void)
{
    CDisplay::PromptSelectStruct prompt_select;
    prompt_select.item_count = 9;
    prompt_select.initial_selection = getValue(g_config.brightness);
    prompt_select.title = F(" 821687 "); // BRIGHT
    type_const_char_ptr item_array[] = {F("  4070  "), F("  537 1 "), F("  537 2 "), // AUTO SET x
                                        F("  537 3 "), F("  537 4 "), F("  537 5 "),
                                        F("  537 6 "), F("  537 7 "), F("  537 8 ")};
    prompt_select.item_array = item_array;
    g_display.SetDisplayBrightness(g_config.brightness); // Set initial brightness
    
    int8_t selection = g_display.PromptSelect(prompt_select, Timeout::SELECT,
    [](CDisplay::Event event, uint8_t selection) -> bool
    {
        switch (event)
        {
        case CDisplay::Event::DECREMENT:
        case CDisplay::Event::INCREMENT:
            if (static_cast<CDisplay::Brightness>(selection) == CDisplay::Brightness::AUTO)
            {
                CDisplay::Brightness brightness;
                brightness = ReadLightIntensity();
                
                if (brightness == CDisplay::Brightness::MIN)
                {
                    brightness = CDisplay::Brightness::L1;
                }
                
                g_display.SetDisplayBrightness(brightness);
            }
            else
            {
                g_display.SetDisplayBrightness(static_cast<CDisplay::Brightness>(selection));
            }
            break;
        
        case CDisplay::Event::SELECTION:
            break;
        case CDisplay::Event::TIMEOUT:
            g_display.SetDisplayBrightness(g_config.brightness); // Restore brightness
            break;
        
        default:
            break;
        }
        
        return false;
    });

    if (selection > -1)
    {
        g_config.brightness = static_cast<decltype(g_config.brightness)>(selection);
        SetConfig(g_config);
        return true;
    }

    return false;
}


bool SetGain(void)
{
    char s[DISPLAY_COUNT + 1];
    CDisplay::PromptValueStruct prompt_value;
    snprintf_P(s, DISPLAY_COUNT + 1, PSTR("   %02u   "), g_config.gain);
    type_const_uint8 item_value[] = {g_config.gain};
    prompt_value.item_count = 1;
    prompt_value.item_position = (const uint8_t []){3};
    prompt_value.item_digit_count = (const uint8_t []){2};
    prompt_value.item_value = item_value;
    prompt_value.item_lower_limit = (const type_const_uint8 []){1};
    prompt_value.item_upper_limit = (const type_const_uint8 []){50};
    prompt_value.initial_display = s;
    prompt_value.title = F("  6410  "); // GAIN

    if (g_display.PromptValue(prompt_value, Timeout::VALUE) > -1)
    {
        g_config.gain = prompt_value.item_value[0];
        SetConfig(g_config);
        return true;
    }

    return false;
}


bool SetOffset(void)
{
    char s[DISPLAY_COUNT + 1];
    CDisplay::PromptValueStruct prompt_value;
    snprintf_P(s, DISPLAY_COUNT + 1, PSTR("   %02u   "), g_config.offset);
    type_const_uint8 item_value[] = {g_config.offset};
    prompt_value.item_count = 1;
    prompt_value.item_position = (const uint8_t []){3};
    prompt_value.item_digit_count = (const uint8_t []){2};
    prompt_value.item_value = item_value;
    prompt_value.item_lower_limit = (const type_const_uint8 []){0};
    prompt_value.item_upper_limit = (const type_const_uint8 []){20};
    prompt_value.initial_display = s;
    prompt_value.title = F(" 044537 "); // OFFSET

    if (g_display.PromptValue(prompt_value, Timeout::VALUE) > -1)
    {
        g_config.offset = prompt_value.item_value[0];
        SetConfig(g_config);
        return true;
    }

    return false;
}


bool SetBlip(void)
{
    CDisplay::PromptSelectStruct prompt_select;
    prompt_select.initial_selection = getValue(g_config.noise);
    prompt_select.title = F(" 802232 "); // BUZZER
    int8_t selection = SelectState(prompt_select);

    if (selection > -1)
    {
        g_config.noise = static_cast<decltype(g_config.noise)>(selection);
        SetConfig(g_config);
        return true;
    }

    return false;
}


bool SetTimeFormat(void)
{
    CDisplay::PromptSelectStruct prompt_select;
    prompt_select.item_count = 2;
    prompt_select.initial_selection = getValue(g_config.time_format);
    prompt_select.title = F("  8002  "); // HOUR
    type_const_char_ptr item_array[] = {F("  ;24:  "), F("  ;12:  ")};
    prompt_select.item_array = item_array;
    int8_t selection = g_display.PromptSelect(prompt_select, Timeout::SELECT);

    if (selection > -1)
    {
        g_config.time_format = static_cast<decltype(g_config.time_format)>(selection);
        SetConfig(g_config);
        return true;
    }

    return false;
}


bool SetDateFormat(void)
{
    CDisplay::PromptSelectStruct prompt_select;
    prompt_select.item_count = 3;
    prompt_select.initial_selection = getValue(g_config.date_format);
    prompt_select.title = F("  2473  "); // DATE
    type_const_char_ptr item_array[] = {F("99:12:31"), F("12:31:99"), F("31:12:99")};
    prompt_select.item_array = item_array;
    int8_t selection = g_display.PromptSelect(prompt_select, Timeout::SELECT);

    if (selection > -1)
    {
        g_config.date_format = static_cast<decltype(g_config.date_format)>(selection);
        SetConfig(g_config);
        return true;
    }

    return false;
}


bool SetTemperatureUnit(void)
{
    CDisplay::PromptSelectStruct prompt_select;
    prompt_select.item_count = 2;
    prompt_select.initial_selection = getValue(g_config.temperature_unit);
    prompt_select.title = F("  7309  "); // TEMP
    type_const_char_ptr item_array[] = {F(" 7309 6 "), F(" 7309 4 ")}; // TEMP C TEMP F
    prompt_select.item_array = item_array;
    int8_t selection = g_display.PromptSelect(prompt_select, Timeout::SELECT);

    if (selection > -1)
    {
        g_config.temperature_unit = static_cast<decltype(g_config.temperature_unit)>(selection);
        SetConfig(g_config);
        return true;
    }

    return false;
}


bool SetTime(void)
{
    char s[DISPLAY_COUNT + 1];
    CDisplay::PromptValueStruct prompt_value;
    CRTC::RTC rtc;
    g_rtc.GetRTC(rtc);
    FormatRTCString(rtc, s, RTCSelect::TIME);
    type_const_uint8 item_value[] = {rtc.hour, rtc.minute, rtc.second};
    prompt_value.item_count = 3;
    prompt_value.item_position = (const uint8_t []){0, 3, 6};
    prompt_value.item_digit_count = (const uint8_t []){2, 2, 2};
    prompt_value.item_value = item_value;
    prompt_value.initial_display = s;
    prompt_value.title = F("  7103  "); // TIME

    if (SelectRTCValue(prompt_value))
    {
        g_rtc.SetTime(prompt_value.item_value[0],
                      prompt_value.item_value[1],
                      prompt_value.item_value[2]);
        return true;
    }

    return false;
}


bool SetDate(void)
{
    char s[DISPLAY_COUNT + 1];
    CDisplay::PromptValueStruct prompt_value;
    CRTC::RTC rtc;
    g_rtc.GetRTC(rtc);
    FormatRTCString(rtc, s, RTCSelect::DATE);
    uint8_t item_value_index[3]; // Each entry represents a format option
    prompt_value.item_count = 3;
    prompt_value.item_position = (const uint8_t []){0, 3, 6};
    prompt_value.item_digit_count = (const uint8_t []){2, 2, 2};
    
    // Dynamically created - Must be kept within scope
    type_const_uint8 item_valueYYMMDD[] = {rtc.year, rtc.month, rtc.day};
    type_const_uint8 item_valueMMDDYY[] = {rtc.month, rtc.day, rtc.year};
    type_const_uint8 item_valueDDMMYY[] = {rtc.day, rtc.month, rtc.year};

    switch (g_config.date_format)
    {
    default:
    case FormatDate::YYMMDD:
        prompt_value.item_upper_limit = (const type_const_uint8 []){99, 12, 31};
        prompt_value.item_lower_limit = (const type_const_uint8 []){0, 1, 1};
        prompt_value.item_value = item_valueYYMMDD;
        memcpy(item_value_index, (const type_const_uint8 []){0, 1, 2}, sizeof(item_value_index));
        break;
        
    case FormatDate::MMDDYY:
        prompt_value.item_upper_limit = (const type_const_uint8 []){12, 31, 99};
        prompt_value.item_lower_limit = (const type_const_uint8 []){1, 1, 0};
        prompt_value.item_value = item_valueMMDDYY;
        memcpy(item_value_index, (const type_const_uint8 []){2, 0, 1}, sizeof(item_value_index));
        break;
        
    case FormatDate::DDMMYY:
        prompt_value.item_upper_limit = (const type_const_uint8 []){31, 12, 99};
        prompt_value.item_lower_limit = (const type_const_uint8 []){1, 1, 0};
        prompt_value.item_value = item_valueDDMMYY;
        memcpy(item_value_index, (const type_const_uint8 []){2, 1, 0}, sizeof(item_value_index));
        break;
    }

    prompt_value.initial_display = s;
    prompt_value.title = F("  2473  "); // DATE

    if (g_display.PromptValue(prompt_value, Timeout::VALUE) > -1)
    {
        g_rtc.SetDate(prompt_value.item_value[item_value_index[0]],
                      prompt_value.item_value[item_value_index[1]],
                      prompt_value.item_value[item_value_index[2]]);

        return true;
    }

    return false;
}


bool SetAlarmState(uint8_t& alarm)
{
    type_const_char_ptr alarm_str[] = {F(" 4;1  0 "), F(" 4;1  1 "),    // A1 OFF, A1 ON
                                       F(" 4;2  0 "), F(" 4;2  1 "),    // A2 OFF, A2 ON
                                       F(" 4;3  0 "), F(" 4;3  1 ")};   // A3 OFF, A3 ON
    uint8_t alarm_index[ALARM_COUNT];
    
    for (uint8_t index = 0; index < ALARM_COUNT; index++)
    {
        bool state = (g_config.alarm[index].state == State::ENABLE);
        alarm_index[index] = (index * 2) + state;
    }
    
    CDisplay::PromptSelectStruct prompt_select;
    prompt_select.item_count = ALARM_COUNT;
    prompt_select.display_mode = CDisplay::Mode::SCROLL;
    
    type_const_char_ptr item_array[ALARM_COUNT] = {alarm_str[alarm_index[0]],
                                                   alarm_str[alarm_index[1]],
                                                   alarm_str[alarm_index[2]]};
    
    prompt_select.item_array = item_array;
    int8_t selection_alarm = g_display.PromptSelect(prompt_select, Timeout::SELECT);

    if (selection_alarm > -1)
    {
        prompt_select.initial_selection = getValue(g_config.alarm[selection_alarm].state);
        int8_t selection_state = SelectState(prompt_select);
        
        if (selection_state > -1)
        {
            // Check if alarm is enabled
            if (selection_state)
            {
                // Capture alarm selection
                alarm = (uint8_t)selection_alarm;
                // Set alarm if any days are enabled
                g_config.alarm[alarm].state = (g_config.alarm[alarm].days == 0)
                                            ? State::DISABLE : State::ENABLE;
            }
            else
            {
                // Disable alarm
                g_config.alarm[selection_alarm].state = static_cast<decltype(g_config.alarm[0].state)>(selection_state);
            }

            SetConfig(g_config);
            
            // Additional processing required if alarm enabled
            return selection_state;
        }
    }

    return false;
}


bool SetAlarmTime(const uint8_t alarm)
{
    char s[DISPLAY_COUNT + 1];
    CDisplay::PromptValueStruct prompt_value;
    
    uint8_t hour = g_config.alarm[alarm].time / 3600;
    uint8_t minute = ((g_config.alarm[alarm].time / 60) % 60);
    
    snprintf_P(s, DISPLAY_COUNT + 1, PSTR("  %02u%02u  "), FormatHour(hour), minute);
    type_const_uint8 item_value[] = {hour, minute};
    prompt_value.item_count = 2;
    prompt_value.item_position = (const uint8_t []){2, 4};
    prompt_value.item_digit_count = (const uint8_t []){2, 2};
    prompt_value.item_value = item_value;
    prompt_value.initial_display = s;
    prompt_value.title = F("  537   "); // SET

    if (SelectRTCValue(prompt_value))
    {
        // Convert alarm to seconds
        g_config.alarm[alarm].time = GetSeconds(prompt_value.item_value[0], prompt_value.item_value[1], 0);
        SetConfig(g_config);
        return true;
    }

    return false;
}


bool SetAlarmDays(const uint8_t alarm)
{
    int8_t selection = -1;

    do
    {
        selection++;
        CDisplay::PromptSelectStruct prompt_select;
        prompt_select.item_count = 8;
        prompt_select.initial_selection = selection;
        prompt_select.display_mode = CDisplay::Mode::SCROLL;
        type_const_char_ptr item_array[] = {F("500249 1"), F("000249 2"), F("703579 3"), // SUNDAY MONDAY TUESDAY
                                            F("320529 4"), F("782529 5"), F("421249 6"), // WDNSDY THRSDY FRIDAY
                                            F("547429 7"), F("  2003  ")};               // SATRDY DONE
        prompt_select.item_array = item_array;
        selection = g_display.PromptSelect(prompt_select, Timeout::SELECT);

        if ((selection < 7) && (selection > -1))
        {
            CDisplay::PromptSelectStruct prompt_select_e;
            prompt_select_e.initial_selection = ((g_config.alarm_state >> (selection + 1)) & 0x1);
            int8_t state = SelectState(prompt_select_e);

            if (state > -1)
            {
                g_config.alarm[alarm].days ^= (-state ^ g_config.alarm[alarm].days) & (0x1 << (selection + 1));
                g_config.alarm[alarm].state = (g_config.alarm[alarm].days == 0) ? State::DISABLE : State::ENABLE;
                SetConfig(g_config);
            }
            else
            {
                return false;
            }
        }
    } while ((selection < 7) && (selection > -1));

    if (selection > -1)
    {
        return true;
    }

    return false;
}

    
bool SetMusic(uint8_t& music)
{
    char s[DISPLAY_COUNT + 1];
    CDisplay::PromptValueStruct prompt_value;
    snprintf_P(s, DISPLAY_COUNT + 1, PSTR(" 537 %02u "), music); // SET
    type_const_uint8 item_value[] = {music};
    type_const_uint8 song_entries = g_song_entries - 1;
    type_const_uint8 item_upper_limit[] = {song_entries};
    prompt_value.item_count = 1;
    prompt_value.brightness_min = CDisplay::Brightness::MAX;
    prompt_value.item_position = (const uint8_t []){5};
    prompt_value.item_digit_count = (const uint8_t []){2};
    prompt_value.item_value = item_value;
    prompt_value.item_lower_limit = (const type_const_uint8 []){0};
    prompt_value.item_upper_limit = item_upper_limit;
    prompt_value.initial_display = s;
    prompt_value.title = F(" 40210  "); // AUDIO

    InterruptSpeed(INTERRUPT_SLOW);
    
    g_display.PromptValue(prompt_value, Timeout::VALUE,
    [&music](CDisplay::Event event, uint8_t selection) -> bool
    {
        switch (event)
        {
        case CDisplay::Event::DECREMENT:
        case CDisplay::Event::INCREMENT:
            g_audio.Stop(); // Mute audio
            PlayMusic(selection);
            break;
        
        case CDisplay::Event::SELECTION:
            music = selection;
            SetConfig(g_config);
            g_audio.Stop(); // Mute audio
            break;

        case CDisplay::Event::TIMEOUT:
            //Don't time out during playback
            if (g_audio.IsActive())
            {
                return true;
            }

            g_audio.Stop(); // Mute audio
            break;

        default:
            break;
        }
        
        return false;
    });
    
    InterruptSpeed(INTERRUPT_FAST);

    return true;
}


void SetTimer(void)
{
    uint32_t timer = 500;
    char s[DISPLAY_COUNT + 1];
    CDisplay::PromptValueStruct prompt_value;
    uint8_t value0 = (timer / 10000);
    uint8_t value1 = (timer / 100) % 100;
    uint8_t value2 = (timer % 100);
    type_const_uint8 item_value[] = {value0, value1, value2};
    snprintf_P(s, DISPLAY_COUNT + 1, PSTR("%02u %02u %02u"), value0, value1, value2);
    prompt_value.item_count = 3;
    prompt_value.item_position = (const uint8_t []){0, 3, 6};
    prompt_value.item_digit_count = (const uint8_t []){2, 2, 2};
    prompt_value.item_lower_limit = (const type_const_uint8 []){0, 0, 0};
    prompt_value.item_upper_limit = (const type_const_uint8 []){23, 59, 59};
    prompt_value.item_value = item_value;
    prompt_value.initial_display = s;
    prompt_value.title = F("  537   "); // SET

    if (g_display.PromptValue(prompt_value, Timeout::VALUE) > -1)
    {
        Timer(prompt_value.item_value[0],
              prompt_value.item_value[1],
              prompt_value.item_value[2]);
    }
}


long SetWorldLine(void)
{
    CDisplay::PromptValueStruct prompt_value;
    type_item item_value[] = {0, 0, 0, 0, 0, 0, 0};
    prompt_value.item_count = 7;
    prompt_value.brightness_min = CDisplay::Brightness::L3;
    prompt_value.item_position = (uint8_t *)(const uint8_t []){0, 2, 3, 4, 5, 6, 7};
    prompt_value.item_digit_count = (uint8_t *)(const uint8_t []){1, 1, 1, 1, 1, 1, 1};
    prompt_value.item_lower_limit = (type_item *)(const type_item []){0, 0, 0, 0, 0, 0, 0};
    prompt_value.item_upper_limit = (type_item *)(const type_item []){10, 9, 9, 9, 9, 9, 9};
    prompt_value.item_value = item_value;
    prompt_value.initial_display = "0:000000";
    prompt_value.title = F("  537   ");

    int8_t result = g_display.PromptValue(prompt_value, Timeout::VALUE,
    [&prompt_value](CDisplay::Event event, uint8_t selection) -> bool
    {
        switch (event)
        {
        case CDisplay::Event::DECREMENT:
        case CDisplay::Event::INCREMENT:
            if (prompt_value.item_value[0] == 10)
            {
                g_display.SetUnitValue(0, ' ');
            }
            break;
        
        case CDisplay::Event::SELECTION:
            (void)selection; // Silence compiler warning
            break;

        case CDisplay::Event::TIMEOUT:
            break;

        default:
            break;
        }
        
        return false;
    });
    
    if (result > -1)
    {
        char s[DISPLAY_COUNT + 1];
        g_display.GetDisplayValue(s);
        s[1] = '0';

        long value;
        value = atol(s);

        if (prompt_value.item_value[0] > 9)
        {
            value += 100000000;
        }

        return value;
    }

    return -1;
}
