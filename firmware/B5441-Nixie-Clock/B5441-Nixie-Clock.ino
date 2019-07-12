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
 * @file        B5441-Nixie-Clock.ino
 * @summary     Digital Clock for B5441 Nixie display tubes
 * @version     3.2
 * @author      nitacku
 * @data        14 August 2018
 */

#include "B5441-Nixie-Clock.h"
#include "Menu.h"

//---------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------

// Object variables
StateStruct     g_state;
Config          g_config;
CDS3232         g_rtc;
CAudio          g_audio{DIGITAL_PIN_TRANSDUCER_0, DIGITAL_PIN_TRANSDUCER_1};
CDisplay        g_display{DISPLAY_COUNT};

// Container variables
CRTC::RTC*      g_rtc_struct;

// Integral variables
uint8_t         g_song_entries = INBUILT_SONG_COUNT;

// Input variables
volatile bool g_button_update = false;
volatile uint8_t g_button_timeout_A = 0;
volatile uint16_t g_button_timeout_B = 0;
volatile uint8_t g_button_timeout = 0;

//---------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------

void loop(void)
{
    CRTC::RTC rtc; // struct
    uint8_t previous_second;
    char s[DISPLAY_COUNT + 1];
    
    g_rtc_struct = &rtc; // Assign global pointer
    
    GetConfig(g_config);

    if (g_config.validate != CONFIG_KEY)
    {
        Config new_config; // Use default constructor values
        SetConfig(new_config); // Write to EEPROM
        GetConfig(g_config); // Read from EEPROM
    }
    
    // Initialize Display
    g_display.SetCallbackIsIncrement(IsInputIncrement);
    g_display.SetCallbackIsSelect(IsInputSelect);
    g_display.SetCallbackIsUpdate(IsInputUpdate);
    g_display.SetDisplayBrightness(g_config.brightness);
    InterruptSpeed(INTERRUPT_FAST);
    delay(1); // Wait for interrupt to occur
    DisplayState(State::ENABLE); // Enable voltage after update
    
    // Initialize RTC
    g_rtc.Initialize();
    UpdateAlarmIndicator();

    while (true)
    {
        AutoBrightness();
        previous_second = rtc.second;
        g_rtc.GetRTC(rtc);
        
        if (rtc.second != previous_second)
        {
            switch (rtc.second)
            {
            case 0:
                AutoBlanking();
                AutoAlarm();

                g_display.SetDisplayIndicator(false);
                g_display.EffectScroll(F(";:;:;:;:"), CDisplay::Direction::LEFT, 80);
                g_rtc.GetRTC(rtc); // Refresh RTC
                FormatRTCString(rtc, s, RTCSelect::TIME);
                g_display.EffectScroll(s, CDisplay::Direction::LEFT, 80);
                break;

            case 30:
                g_display.SetDisplayIndicator(false);
                FormatRTCString(rtc, s, RTCSelect::DATE);
                g_display.SetDisplayValue(s);
                g_display.EffectSlotMachine(44);
                delay(1950);
                break;

            default:
                bool pip = (!rtc.am && (g_config.time_format == FormatTime::H12));
                g_display.SetUnitIndicator(0, getValue(g_state.alarm)); // Alarm
                g_display.SetUnitIndicator(5, pip); // AM/PM
                FormatRTCString(rtc, s, RTCSelect::TIME);
                g_display.SetDisplayValue(s);
                break;
            }
        }

        if (IsInputUpdate() || IsInputSelect())
        {
            // Check if time threshold elapsed
            if (g_button_timeout == 0)
            {
                // Check if display is disabled
                if (g_state.display == State::DISABLE)
                {
                    DisplayState(State::ENABLE);
                }
                else
                {
                    g_display.SetDisplayIndicator(false);
                    
                    // Check if button was pressed
                    if (IsInputSelect())
                    {                        
                        FormatRTCString(rtc, s, RTCSelect::DATE);
                        g_display.SetDisplayValue(s);
                        MenuInfo();
                    }
                    else
                    {
                        MenuSettings();
                    }
                    
                    UpdateAlarmIndicator();
                }
            }
            
            g_button_timeout = 10;
        }

        if (g_button_timeout)
        {
            g_button_timeout--;
        }
        
        delay(50); // Idle
    }
}


void EffectWorldLine(const uint32_t value, const uint8_t omega)
{
    g_display.SetDisplayBrightness(CDisplay::Brightness::L5);
    g_display.SetDisplayValue(value);
    g_display.SetUnitValue(1, ':');

    if ((value >= 100000000) || (omega && (value < 600000)))
    {
        g_display.SetUnitValue(0, ' ');
    }

    g_display.EffectSlotMachine(29);
    g_display.SetDisplayBrightness(CDisplay::Brightness::L8);
    delay(120);
    g_display.SetDisplayBrightness(CDisplay::Brightness::L7);
    delay(47);
    g_display.SetDisplayBrightness(CDisplay::Brightness::L6);
    delay(47);
    g_display.SetDisplayBrightness(CDisplay::Brightness::L5);
}


void DivergenceMeter(void)
{
    uint8_t meter_mode = 0;
    uint8_t continuous = 0;
    uint8_t cycle = 0;
    bool custom_initialized = false;
    long custom_world_line = 0;

    while (1)
    {
        uint32_t count;
        uint32_t timeout;

        switch (meter_mode)
        {
            case 1:
            EffectWorldLine(world_line[cycle++], 0);

            if (cycle > 14)
            {
                cycle = 0;
            }
            break;

            case 2:
            if (custom_initialized)
            {
                if (++cycle % 2)
                {
                    EffectWorldLine(random(0, 13000000), 1);
                }
                else
                {
                    EffectWorldLine(custom_world_line, 0);
                }

                break;
            }

            custom_world_line = SetWorldLine();

            if (custom_world_line > -1)
            {
                custom_initialized = true;
                cycle = 0;
                EffectWorldLine(custom_world_line, 0);
                break;
            }
            else
            {
                meter_mode = 0;
                continuous = 0;
            }
            [[gnu::fallthrough]]; // Fall-through
            
            case 0: // alpha 76.92%; beta 18.46%; omega 4.62%
            default:
            cycle = 0;
            meter_mode = 0;
            custom_initialized = false;
            EffectWorldLine(random(0, 13000000), 1);
            break;
        }

        count = 250000; // 3 second delay
        timeout = 25000000; // timeout after 5.5 minutes of inactivity

        IsInputUpdate(); // Clear any update
        while (timeout && count && !IsInputUpdate() && !IsInputSelect())
        {
            count -= continuous;
            timeout--;
        }

        if (count)
        {
            if (!IsInputSelect())
            {
                count = 100000;

                while (count && (digitalRead(BUTTON_B) == HIGH))
                {
                    count--;
                }

                if (count)
                {
                    continuous = 0;
                }
                else
                {
                    continuous = continuous ? 0 : 1;
                }
            }
            else
            {
                count = 100000;

                while (count && IsInputSelect())
                {
                    count--;
                }

                if (count)
                {
                    continuous = 0;
                    meter_mode++;
                }
                else
                {
                    g_display.SetDisplayBrightness(g_config.brightness);
                    return;
                }
            }
        }

        if (!timeout)
        {
            g_display.SetDisplayBrightness(g_config.brightness);
            return;
        }
    }
}


void Timer(uint8_t hour, uint8_t minute, uint8_t second)
{
    const char* format_string = PSTR("%02u%c%02u%c%02u");
    char s[DISPLAY_COUNT + 1];
    uint8_t previous_second;

    CRTC::RTC rtc;
    g_rtc.GetRTC(rtc);
    previous_second = rtc.second;

    while ((second + minute + hour) > 0)
    {
        if (previous_second != rtc.second)
        {
            if (second == 0)
            {
                if (minute == 0)
                {
                    hour--;
                    minute = 59; // Reset
                }
                
                minute--;
                second = 59; // Reset
            }
            
            second--;
            previous_second = rtc.second;
            
            char c = ((rtc.second & 0x1) ? ';' : ':');
            snprintf_P(s, DISPLAY_COUNT + 1, format_string, hour, c, minute, c, second);
            g_display.SetDisplayValue(s);
        }

        delay(50);
        g_rtc.GetRTC(rtc);

        if (IsInputSelect())
        {
            return;
        }
    }

    PlayAlarm(g_config.music_timer, "00000000");
}


void Detonate(void)
{
    uint32_t countdown = 99999;
    uint16_t value = 2500;
    uint16_t timer = 0;
    uint8_t count = 0;

    ButtonState(State::DISABLE); // Disable buttons
    g_display.SetDisplayValue(countdown);
    g_display.SetDisplayBrightness(CDisplay::Brightness::MAX);
    InterruptSpeed(INTERRUPT_SLOW);
    
    delay(500);
    g_audio.Play(CAudio::Functions::PGMStream, music_detonate_begin, music_detonate_begin);
    delay(500);

    do
    {
        g_display.SetDisplayValue(countdown);

        if (++timer >= value)
        {
            count++;
            value -= 50;
            value += count / 2;
            timer = 0;
            countdown -= 150;
            g_audio.Play(CAudio::Functions::PGMStream, music_detonate_beep, music_detonate_beep);
        }
    } while (--countdown > 150);

    g_display.SetDisplayValue(F("00000000"));
    g_audio.Play(CAudio::Functions::PGMStream, music_detonate_fuse_A, music_detonate_fuse_A);
    delay(500);

    for (uint8_t i = 0; i < 3; i++)
    {
        g_audio.Play(CAudio::Functions::PGMStream, music_detonate_fuse_B, music_detonate_fuse_B);
        
        for (uint8_t j = 0; j < 50; j++)
        {
            for (uint8_t k = 0; k < DISPLAY_COUNT; k++)
            {
                uint8_t digit = random(0, DISPLAY_COUNT);
                g_display.SetUnitValue(digit, '<');
            }
            
            delay(20);
            g_display.SetDisplayValue(F("00000000")); // Restore
        }
    }
    
    g_display.SetDisplayValue(F("<<<<<<<<")); // Connect all anodes
    g_audio.Play(CAudio::Functions::PGMStream, music_detonate_end_A, music_detonate_end_B);
    delay(1000);
    g_display.SetDisplayValue(F("        "));
    delay(3000);
    ButtonState(State::ENABLE); // Enable buttons
    InterruptSpeed(INTERRUPT_FAST);
    g_display.SetDisplayBrightness(g_config.brightness);
}


void PlayAlarm(const uint8_t song_index, const char* phrase)
{
    uint8_t elapsed_seconds = 0;
    bool toggle_state = false;
    bool audio_active = false;

    DisplayState(State::ENABLE);
    g_display.SetDisplayIndicator(false);
    g_display.SetDisplayBrightness(CDisplay::Brightness::MAX);
    InterruptSpeed(INTERRUPT_SLOW);
    
    // Alarm for at least 120 seconds until music ends or until user interrupt
    do
    {
        if (!audio_active)
        {
            PlayMusic(song_index);
        }
        
        if (g_rtc.GetTimeSeconds() & 0x1)
        {
            if (toggle_state == true)
            {
                toggle_state = false;
                elapsed_seconds++;
                g_display.SetDisplayValue(phrase);
            }
        }
        else
        {
            if (toggle_state == false)
            {
                toggle_state = true;
                elapsed_seconds++;
                g_display.SetDisplayValue(F("        "));
            }
        }

        delay(50);
        audio_active = g_audio.IsActive();
        
    } while (((elapsed_seconds < 120) || audio_active) &&
             !(IsInputUpdate() || IsInputSelect()));
            
    g_audio.Stop(); // Ensure music is stopped
    
    g_button_timeout = 10;
    InterruptSpeed(INTERRUPT_FAST);
    g_display.SetDisplayBrightness(g_config.brightness);
}


void AutoBrightness(void)
{
    if (g_config.brightness == CDisplay::Brightness::AUTO)
    {
        CDisplay::Brightness brightness;
        brightness = ReadLightIntensity();
        g_display.SetDisplayBrightness(brightness);
    }
}


void AutoBlanking(void)
{
    if (g_config.blank_begin != g_config.blank_end)
    {
        uint32_t seconds = g_rtc.GetTimeSeconds();

        if (seconds == g_config.blank_end)
        {
            DisplayState(State::ENABLE);
        }
        else if (seconds == g_config.blank_begin)
        {
            DisplayState(State::DISABLE);
        }
    }
}


void AutoAlarm(void)
{
    uint32_t current_time = GetSeconds(g_rtc_struct->hour, g_rtc_struct->minute, 0);
    
    for (uint8_t index = 0; index < ALARM_COUNT; index++)
    {
        // Check if alarm is enabled
        if (g_config.alarm[index].state == State::ENABLE)
        {
            // Check if alarm day matches current day
            if ((g_config.alarm[index].days >> g_rtc_struct->week_day) & 0x1)
            {
                // Check if alarm time matches current time
                if (g_config.alarm[index].time == current_time)
                {
                    PlayAlarm(g_config.alarm[index].music, "1.048596");
                    break; // No need to process remaining alarms
                }
            }
        }
    }
    
    UpdateAlarmIndicator();
}


void UpdateAlarmIndicator(void)
{
    CRTC::RTC rtc;
    uint8_t day;
    State next_state = State::DISABLE; // Assume disabled
    
    g_rtc.GetRTC(rtc);
    uint32_t current_time = GetSeconds(rtc.hour, rtc.minute, 0);
    
    for (uint8_t index = 0; index < ALARM_COUNT; index++)
    {
        // Check if alarm is enabled
        if (g_config.alarm[index].state == State::ENABLE)
        {
            // Determine which day to check
            if (g_config.alarm[index].time > current_time)
            {
                day = rtc.week_day; // Today
            }
            else
            {
                day = (rtc.week_day > 6) ? 1 : (rtc.week_day + 1); // Tomorrow
            }

            // Check if alarm is active for selected day
            if ((g_config.alarm[index].days >> day) & 0x1)
            {
                next_state = State::ENABLE;
                break; // Alarm will be indicated - no need to continue
            }
        }
    }

    // Update alarm indicator
    g_state.alarm = static_cast<decltype(g_state.alarm)>(next_state);
}


uint8_t FormatHour(const uint8_t hour)
{
    if (g_config.time_format == FormatTime::H24)
    {
        return hour;
    }
    else
    {
        uint8_t conversion = hour % 12;
        conversion += (conversion == 0) ? 12 : 0;
        return conversion;
    }
}


void FormatRTCString(const CRTC::RTC& rtc, char* s, const RTCSelect type)
{
    const char* format_string = PSTR("%02u%c%02u%c%02u");
    char c;

    if (type == RTCSelect::TIME)
    {
        c = ((rtc.second & 0x1) ? ';' : ':');
    }
    else
    {
        c = ' ';
    }
    
    switch (type)
    {
    case RTCSelect::TIME:
        snprintf_P(s, DISPLAY_COUNT + 1, format_string, FormatHour(rtc.hour), c, rtc.minute, c, rtc.second);
        break;

    case RTCSelect::DATE:
        switch (g_config.date_format)
        {
        default:
        case FormatDate::YYMMDD:
            snprintf_P(s, DISPLAY_COUNT + 1, format_string, rtc.year, c, rtc.month, c, rtc.day);
            break;

        case FormatDate::MMDDYY:
            snprintf_P(s, DISPLAY_COUNT + 1, format_string, rtc.month, c, rtc.day, c, rtc.year);
            break;

        case FormatDate::DDMMYY:
            snprintf_P(s, DISPLAY_COUNT + 1, format_string, rtc.day, c, rtc.month, c, rtc.year);
            break;
        }

        break;
    }
}


uint32_t GetSeconds(const uint8_t hour, const uint8_t minute, const uint8_t second)
{
    return ((3600 * (uint32_t)hour) + (60 * (uint32_t)minute) + (uint32_t)second);
}


CDisplay::Brightness ReadLightIntensity(void)
{
    const uint8_t SAMPLES = 32;
    static uint16_t history[SAMPLES];
    static uint8_t result = 0;
    static uint32_t previous_average = 0;
    uint32_t average = 0;
    int32_t difference;

    for (uint8_t index = 0; index < (SAMPLES - 1); index++)
    {
        average += (history[index] = history[index + 1]);
    }

    average += (history[SAMPLES - 1] = (analogRead(ANALOG_PIN_PHOTODIODE - A0) + g_config.offset));
    average /= SAMPLES;
    average *= g_config.gain;
    average /= 10; // pseudo float
    difference = (average - previous_average);
    difference = (difference > 0) ? difference : -difference;

    if (difference > 3)
    {
        uint8_t value;
        
        if (average < 100)
        {
            value = 1 + (average / 25);
        }
        else
        {
            value = 2 + (average / 100);
        }

        uint8_t max = getValue(CDisplay::Brightness::MAX);

        if (value > max)
        {
            value = max;
        }
        
        result = value;
        previous_average = average;
    }

    return static_cast<CDisplay::Brightness>(result);
}


void GetConfig(Config& config)
{
    while (!eeprom_is_ready());
    cli();
    eeprom_read_block((void*)&config, (void*)0, sizeof(Config));
    sei();
}


void SetConfig(const Config& config)
{
    while (!eeprom_is_ready());
    cli();
    eeprom_update_block((const void*)&config, (void*)0, sizeof(Config));
    sei();
}


void VoltageState(State state)
{
    g_state.voltage = state;
    digitalWrite(DIGITAL_PIN_HV_ENABLE, getValue(state));
}


void DisplayState(State state)
{
    g_state.display = state;
    digitalWrite(DIGITAL_PIN_BLANK, getValue(state));
    VoltageState(state);
}


void ButtonState(const State state)
{
    if (state == State::ENABLE)
    {
        // Attach button hardware interrupts
        attachInterrupt(digitalPinToInterrupt(BUTTON_A), ISR_button_A, RISING);
        attachInterrupt(digitalPinToInterrupt(BUTTON_B), ISR_button_B, RISING);
    }
    else
    {
        detachInterrupt(digitalPinToInterrupt(BUTTON_A));
        detachInterrupt(digitalPinToInterrupt(BUTTON_B));
    }
}


void InterruptSpeed(const uint8_t speed)
{
    // set compare match register for xHz increments
    OCR2A = speed; // = (16MHz) / (x*prescale) - 1 (must be <255)
}


bool IsInputIncrement(void)
{
    return true; // Always increment
}


bool IsInputSelect(void)
{
    return (digitalRead(BUTTON_A) == HIGH);
}


bool IsInputUpdate(void)
{
    bool current_value = g_button_update;
    g_button_update = false; // Reset
    return current_value;
}


void ISR_button_A(void)
{
    g_button_timeout_A = 75; // Debounce milliseconds
}


__attribute__((optimize("-O3")))
void ISR_button_B(void)
{
    if (g_button_timeout_B == 0)
    {
        if (g_config.noise == State::ENABLE)
        {
            g_audio.Play(CAudio::Functions::MemStream, music_blip, music_blip);
        }
        
        g_button_update = true; // Update only after idle
    }
    
    g_button_timeout_B = 75; // Debounce milliseconds
}


// Interrupt is called every millisecond
ISR(TIMER0_COMPA_vect) 
{
    static bool initial = true;
    wdt_reset(); // Reset watchdog timer
    
    // Decrement button timeout when button is low
    if ((g_button_timeout_A > 0) && (digitalRead(BUTTON_A) == LOW))
    {
        g_button_timeout_A--;
    }
    
    if (digitalRead(BUTTON_B) == HIGH)
    {
        g_button_timeout_B--;
        
        // Support auto repeat after holding 
        if (g_button_timeout_B == 0)
        {   
            if (initial)
            {
                initial = false;
                g_button_timeout_B = 1000; // Reload
            }
            else
            {
                g_button_update = true;
                g_button_timeout_B = 200; // Reload
            }
        }
    }
    else
    {
        if (g_button_timeout_B > 0)
        {
            if (g_button_timeout_B > 75)
            {
                g_button_timeout_B = 75; // Speed up decay
            }
            
            g_button_timeout_B--;
        }
        
        initial = true; // Reset
    }
}


ISR(TIMER2_COMPA_vect)
{
    static uint8_t pwm_cycle = 0;
    static const uint8_t toggle[] = {0xFF, 0x01, 0x11, 0x25, 0x55, 0x5B, 0x77, 0x7F, 0xFF};
    static const uint8_t pinout[12] = {10, 1, 2, 6, 7, 8, 9, 4, 5, 3, 0, 11};

    sei(); // Enable interrupts for audio processing
    
    // No need to update display if disabled
    if (g_state.display == State::DISABLE)
    {
        return;
    }

    pwm_cycle++;

    if (pwm_cycle > 7)
    {
        pwm_cycle = 0;
    }

    setPinLow(DIGITAL_PIN_LATCH); // latch

    for (uint8_t tube = 0; tube < DISPLAY_COUNT; tube++)
    {
        uint16_t digit_bitmap = 0;

        if ((toggle[getValue(g_display.GetUnitBrightness(tube))] >> pwm_cycle) & 0x1)
        {
            uint8_t unit = g_display.GetUnitValue(tube) - '0';
            uint8_t indicator = g_display.GetUnitIndicator(tube);
            
            if (unit < sizeof(pinout))
            {
                digit_bitmap = (1 << pinout[unit]) | indicator;
            }
            else
            {
                if (unit == sizeof(pinout))
                {
                    digit_bitmap = 0xFF; // Connect all anodes
                }
            }
        }

        for (uint8_t index = 0; index < sizeof(pinout); index++)
        {
            setPinHigh(DIGITAL_PIN_CLOCK); // clock

            if ((digit_bitmap >> index) & 0x1)
            {
                setPinHigh(DIGITAL_PIN_SDATA); // sdata
            }
            else
            {
                setPinLow(DIGITAL_PIN_SDATA); // sdata
            }

            setPinLow(DIGITAL_PIN_CLOCK); // clock
        }
    }

    setPinHigh(DIGITAL_PIN_LATCH); // latch
}


void setup(void)
{
    pinMode(DIGITAL_PIN_CLOCK, OUTPUT);         // Clock
    pinMode(DIGITAL_PIN_SDATA, OUTPUT);         // Serial Data
    pinMode(DIGITAL_PIN_LATCH, OUTPUT);         // Latch Enable
    pinMode(DIGITAL_PIN_BLANK, OUTPUT);         // Blank
    pinMode(DIGITAL_PIN_HV_ENABLE, OUTPUT);     // High Voltage Enable
    pinMode(ANALOG_PIN_PHOTODIODE, INPUT);      // Photodiode Voltage
    pinMode(DIGITAL_PIN_TRANSDUCER_0, OUTPUT);  // Transducer A
    pinMode(DIGITAL_PIN_TRANSDUCER_1, OUTPUT);  // Transducer B
    pinMode(DIGITAL_PIN_TRANSDUCER_2, OUTPUT);  // Transducer B
    pinMode(DIGITAL_PIN_BUTTON_0, INPUT);       // Button 0
    pinMode(DIGITAL_PIN_BUTTON_1, INPUT);       // Button 1
    
    // Attach button hardware interrupts
    ButtonState(State::ENABLE);
    
    // Watchdog timer
    wdt_enable(WDTO_1S); // Set for 1 second
    
    // Millisecond timer
    OCR0A = 0x7D;
    TIMSK0 |= _BV(OCIE0A);
    
    // Configure Timer2 (Display)
    TCCR2A = 0; // Reset register
    TCCR2B = 0; // Reset register
    TCNT2  = 0; // Initialize counter value to 0
    //OCR2A set by InterruptSpeed() function - 8 bit register
    TCCR2A |= _BV(WGM21); // Enable CTC mode
    TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20); // Set for 1024 prescaler
    TIMSK2 |= _BV(OCIE2A); // Enable timer compare interrupt
}
