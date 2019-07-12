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
 * @file        Menu.h
 * @summary     Digital Clock for B5441 Nixie display tubes
 * @version     3.2
 * @author      nitacku
 * @data        14 August 2018
 */

#ifndef _MENU_H
#define _MENU_H
 
#include "B5441-Nixie-Clock.h"

typedef type_array type_const_char_ptr;
typedef type_item type_const_uint8;

enum Timeout : uint32_t
{
    INFO   = 200000,
    MENU   =    100,
    SELECT =    500,
    VALUE  =   5000,
};

enum MENU_ITEM : uint8_t
{
    MENU_ITEM_DIVERGENCE,
    MENU_ITEM_TIMER,
    MENU_ITEM_MUSIC,
    MENU_ITEM_BRIGHTNESS,
    MENU_ITEM_BLANK,
    MENU_ITEM_ALARM,
    MENU_ITEM_CONFIG,
    MENU_ITEM_TIME,
    MENU_ITEM_DATE,
    MENU_ITEM_COUNT, // Number of menu items
};

// Preferred character conversion
// A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
// 4 8 6 2 3 4 6 8 1 3 8 1 0 0 0 9 2 2 5 7 0 0 3 8 9 2
// _ _     _   _   _     _     _ _ _ _ _ _         _ _

static const char menu_item_DIVERGENCE[] PROGMEM    = "21046063";
static const char menu_item_TIMER[] PROGMEM         = " 71037  ";
static const char menu_item_MUSIC[] PROGMEM         = " 40210  ";
static const char menu_item_BRIGHTNESS[] PROGMEM    = " 821687 ";
static const char menu_item_BLANK[] PROGMEM         = "2159149 ";
static const char menu_item_ALARM[] PROGMEM         = " 41423  ";
static const char menu_item_CONFIG[] PROGMEM        = "53771065";
static const char menu_item_TIME[] PROGMEM          = "  7103  ";
static const char menu_item_DATE[] PROGMEM          = "  2473  ";

static PGM_P const menu_item_array[] PROGMEM =
{
    [MENU_ITEM_DIVERGENCE] = menu_item_DIVERGENCE,
    [MENU_ITEM_TIMER] = menu_item_TIMER,
    [MENU_ITEM_MUSIC] = menu_item_MUSIC,
    [MENU_ITEM_BRIGHTNESS] = menu_item_BRIGHTNESS,
    [MENU_ITEM_BLANK] = menu_item_BLANK,
    [MENU_ITEM_ALARM] = menu_item_ALARM,
    [MENU_ITEM_CONFIG] = menu_item_CONFIG,
    [MENU_ITEM_TIME] = menu_item_TIME,
    [MENU_ITEM_DATE] = menu_item_DATE,
};

void MenuInfo(void);
void MenuSettings(void);
int8_t SelectCycle(const Cycle init_value);
int8_t SelectState(CDisplay::PromptSelectStruct& prompt_select);
bool SelectRTCValue(CDisplay::PromptValueStruct& prompt_value);
bool RestoreOutOfBox(void);
bool SetBlank(void);
bool SetBrightness(void);
bool SetGain(void);
bool SetOffset(void);
bool SetTimeFormat(void);
bool SetDateFormat(void);
bool SetTemperatureUnit(void);
bool SetBlip(void);
bool SetTime(void);
bool SetDate(void);
bool SetAlarmState(uint8_t& alarm);
bool SetAlarmTime(const uint8_t alarm);
bool SetAlarmDays(const uint8_t alarm);
bool SetMusic(uint8_t& music);
void SetTimer(void);
long SetWorldLine(void);

#endif
