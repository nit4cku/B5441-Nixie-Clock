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
 * @file        B5441-Nixie-Clock.h
 * @summary     Digital Clock for B5441 Nixie display tubes
 * @version     3.2
 * @author      nitacku
 * @data        14 August 2018
 */

#ifndef _DIVERGENCEMETER_H
#define _DIVERGENCEMETER_H
 
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <DS323x.h>
#include <nDisplay.h>
#include <nAudio.h>
#include <nI2C.h>
#include "Music.h"

const uint8_t VERSION       = 10;
const uint8_t DISPLAY_COUNT = 8;
const char CONFIG_KEY       = '$';
const uint8_t ALARM_COUNT   = 3;

// Macros to simplify port manipulation without additional overhead
#define getPort(pin)    ((pin < 8) ? PORTD : ((pin < A0) ? PORTB : PORTC))
#define getMask(pin)    _BV((pin < 8) ? pin : ((pin < A0) ? pin - 8 : pin - A0))
#define setPinHigh(pin) (getPort(pin) |= getMask(pin))
#define setPinLow(pin)  (getPort(pin) &= ~getMask(pin))

enum digital_pin_t : uint8_t
{
    DIGITAL_PIN_BUTTON_0 = 2,
    DIGITAL_PIN_BUTTON_1 = 3,
    DIGITAL_PIN_CLOCK = 10,
    DIGITAL_PIN_SDATA = 12,
    DIGITAL_PIN_LATCH = 13,
    DIGITAL_PIN_BLANK = 11,
    DIGITAL_PIN_HV_ENABLE = 8,
    DIGITAL_PIN_TRANSDUCER_0 = 5,
    DIGITAL_PIN_TRANSDUCER_1 = 6,
    DIGITAL_PIN_TRANSDUCER_2 = 9, // Not populated
};

enum button_t : uint8_t
{
    BUTTON_A = DIGITAL_PIN_BUTTON_1,
    BUTTON_B = DIGITAL_PIN_BUTTON_0,
};

enum analog_pin_t : uint8_t
{
    ANALOG_PIN_PHOTODIODE = A3,
};

enum interrupt_speed_t : uint8_t
{
    INTERRUPT_FAST = 32, // 16MHz / (60Hz * 8 levels * 1024 prescaler)
    INTERRUPT_SLOW = 255,
};

enum class FormatDate : uint8_t
{
    YYMMDD,
    MMDDYY,
    DDMMYY,
};

enum class FormatTime : uint8_t
{
    H24,
    H12,
};

enum class Cycle : uint8_t
{
    AM,
    PM,
};

enum class RTCSelect : uint8_t
{
    TIME,
    DATE,
};

enum class Effect : uint8_t
{
    DISABLE,
    S0,
    S30,
};

enum class State : bool
{
    DISABLE,
    ENABLE,
};

static const uint32_t world_line[15] =
{
    0xA9D63A, 0x17F8D0,
    0x17F8C7, 0x173E63,
    0x163B07, 0x15818C,
    0x146763, 0x158197,
    0x163B12, 0x173E6B,
    0x17F8E6, 0xA9D55D,
    0xA9D57E, 0x73A83D5,
    0xA89694
};

struct StateStruct
{
    StateStruct()
    : voltage(State::DISABLE)
    , display(State::DISABLE)
    , alarm(State::DISABLE)
    {
        // empty
    }

    State voltage;
    State display;
    State alarm;
};

struct AlarmStruct
{
    AlarmStruct()
    : state(State::DISABLE)
    , music(0)
    , days(0)
    , time(0)
    {
        // empty
    }
    
    State       state;
    uint8_t     music;
    uint8_t     days;
    uint32_t    time;
};

struct Config
{
    Config()
    : validate(CONFIG_KEY)
    , noise(State::ENABLE)
    , alarm_state(0x00)
    , brightness(CDisplay::Brightness::AUTO)
    , gain(10)
    , offset(10)
    , date_format(FormatDate::DDMMYY)
    , time_format(FormatTime::H24)
    , temperature_unit(CRTC::Unit::F)
    , blank_begin(0)
    , blank_end(0)
    , music_timer(0)    
    {
        // Empty
    }

    char                    validate;
    State                   noise;
    uint8_t                 alarm_state;
    CDisplay::Brightness    brightness;
    uint8_t                 gain;
    uint8_t                 offset;
    FormatDate              date_format;
    FormatTime              time_format;
    CRTC::Unit              temperature_unit;
    uint32_t                blank_begin;
    uint32_t                blank_end;
    uint8_t                 music_timer;
    AlarmStruct             alarm[ALARM_COUNT];
};

// Return integral value of Enumeration
template<typename T> constexpr uint8_t getValue(const T e)
{
    return static_cast<uint8_t>(e);
}

//---------------------------------------------------------------------
// Implicit Function Prototypes
//---------------------------------------------------------------------

// delay            B7971
// digitalWrite     Wire
// analogRead       B7971
// pinMode          B7971
// attachInterrupt  CEncoder
// strlen           CDisplay
// strlen_P         CDisplay
// strncpy          CDisplay
// strcpy_P         CDisplay
// snprintf_P       B7971
// memcpy           B7971
// memset           B7971
// memcpy_P         B7971

//---------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------

// Mode functions
void EffectWorldLine(const uint32_t value, const uint8_t omega);
void DivergenceMeter(void);
void Timer(uint8_t hour, uint8_t minute, uint8_t second);
void Detonate(void);
void PlayAlarm(const uint8_t song_index, const char* phrase);

// Automatic functions
void AutoBrightness(void);
void AutoBlanking(void);
void AutoAlarm(void);

// Update functions
void UpdateAlarmIndicator(void);

// Format functions
uint8_t FormatHour(const uint8_t hour);
void FormatRTCString(const CRTC::RTC& rtc, char* s, const RTCSelect type);
uint32_t GetSeconds(const uint8_t hour, const uint8_t minute, const uint8_t second);

// Analog functions
CDisplay::Brightness ReadLightIntensity(void);

// EEPROM functions
void GetConfig(Config& g_config);
void SetConfig(const Config& g_config);

// State functions
void VoltageState(const State state);
void DisplayState(const State state);
void ButtonState(const State state);

// Interrupt functions
void InterruptSpeed(const uint8_t speed);

// Callback functions
bool IsInputIncrement(void);
bool IsInputSelect(void);
bool IsInputUpdate(void);
void ISR_button_A(void);
void ISR_button_B(void);

#endif
