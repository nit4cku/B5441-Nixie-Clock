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
 * @file        Music.h
 * @summary     Music collection for nAudio library
 * @version     1.2
 * @author      nitacku
 * @data        14 August 2018
 */

#ifndef _MUSIC_H
#define _MUSIC_H

#include <nAudio.h>
#include <nDisplay.h>

enum Music : uint16_t
{
    OFFSET = 256,
    ADDRESS_SIZE = 2,
    CHANNEL_COUNT = 2,
};

static const uint8_t music_blip[] = { 1, NC8, DBLIP, END };
static const uint8_t music_menu[] PROGMEM = { 12, NG3, DE, ND4, DS, END };
static const uint8_t music_detonate_begin[] PROGMEM = { 30, NC3, DW, END };
static const uint8_t music_detonate_beep[]  PROGMEM = { 30, NS7, DS, END };
static const uint8_t music_detonate_end_A[] PROGMEM = { 30, NS7, DW, END };
static const uint8_t music_detonate_end_B[] PROGMEM = { 30, NS0, DW, END };

static const uint8_t music_detonate_fuse_A[] PROGMEM =
{
    50, NS7, DW, END,
};

static const uint8_t music_detonate_fuse_B[] PROGMEM =
{
    50, NS7, DS, NS6, NS5, NS4, NS3, NS2, NS1, NS0, END,
};

static const uint8_t music_alarm_beep[] PROGMEM =
{
    15, NC6, DDH, NRS, NC6, NRS, END,
};

static const uint8_t music_alarm_pulse[] PROGMEM =
{
    14, NC6, DE, NRS, NC6, NRS, NC6, NRS, NC6, NRS, DH,
    NC6, DE, NRS, NC6, NRS, NC6, NRS, NC6, NRS, DH, END,
};

static const uint8_t music_Tetris_A_Theme_A[] PROGMEM =
{
     32, NE6, DQ, NB5, DE, NC6, ND6, NE6, DS, ND6, NC6, DE, NB5, NA5, DQ, NA5, DE, NC6,
     NE6, DQ, ND6, DE, NC6, NB5, DDQ, NC6, DE, ND6, DQ, NE6, NC6, NA5, NA5, DH, NRS, DE,
     ND6, DQ, NF6, DE, NA6, DQ, NG6, DE, NF6, NE6, DDQ, NC6, DE, NE6, DQ, ND6, DE,
     NC6, NB5, DQ, NB5, DE, NC6, ND6, DQ, NE6, NC6, NA5, NA5, NRS, NE6, NB5, DE,
     NC6, ND6, NE6, DS, ND6, NC6, DE, NB5, NA5, DQ, NA5, DE, NC6, NE6, DQ, ND6, DE,
     NC6, NB5, DDQ, NC6, DE, ND6, DQ, NE6, NC6, NA5, NA5, DH, NRS, DE, ND6, DQ,
     NF6, DE, NA6, DQ, NG6, DE, NF6, NE6, DDQ, NC6, DE, NE6, DQ, ND6, DE, NC6,
     NB5, DQ, NB5, DE, NC6, ND6, DQ, NE6, NC6, NA5, NA5, NRS, NE6, DH, NC6, ND6,
     NB5, NC6, NA5, NGS5, NB5, DQ, NRS, NE6, DH, NC6, ND6, NB5, NC6, DQ, NE6, NA6, DH,
     NGS6, NRS, NE6, DQ, NB5, DE, NC6, ND6, NE6, DS, ND6, NC6, DE, NB5, NA5, DQ,
     NA5, DE, NC6, NE6, DQ, ND6, DE, NC6, NB5, DDQ, NC6, DE, ND6, DQ, NE6,
     NC6, NA5, NA5, DH, NRS, DE, ND6, DQ, NF6, DE, NA6, DQ, NG6, DE, NF6, NE6, DDQ,
     NC6, DE, NE6, DQ, ND6, DE, NC6, NB5, DQ, NB5, DE, NC6, ND6, DQ, NE6,
     NC6, NA5, NA5, END
};

static const uint8_t music_Tetris_A_Theme_B[] PROGMEM =
{
     32, NB5, DQ, NGS5, DE, NA5, NB5, DQ, NA5, DE, NGS5, NE5, DQ, NE5, DE, NA5, NC6, DQ,
     NB5, DE, NA5, NGS5, NE5, NGS5, NA5, NB5, DQ, NC6, NA5, NE5, NE5, DH, NRS, DE,
     NF5, DQ, NA5, DE, NC6, NC6, DS, NC6, NB5, DE, NA5, NG5, DDQ, NE5, DE, NG5,
     NA5, DS, NG5, NF5, DE, NE5, NGS5, NE5, NGS5, NA5, NB5, NGS5, NC6, NGS5, NA5, NE5,
     NE5, DQ, NE5, NRS, NB5, NGS5, DE, NA5, NB5, DQ, NA5, DE, NGS5, NE5, DQ, NE5, DE,
     NA5, NC6, DQ, NB5, DE, NA5, NGS5, NE5, NGS5, NA5, NB5, DQ, NC6, NA5, NE5,
     NE5, DH, NRS, DE, NF5, DQ, NA5, DE, NC6, NC6, DS, NC6, NB5, DE, NA5, NG5, DDQ,
     NE5, DE, NG5, NA5, DS, NG5, NF5, DE, NE5, NGS5, NE5, NGS5, NA5, NB5, NGS5,
     NC6, NGS5, NA5, NE5, NE5, DQ, NE5, NRS, NC6, DH, NA5, NB5, NGS5, NA5, NE5, NE5,
     NGS5, DQ, NRS, NC6, DH, NA5, NB5, NGS5, NA5, DQ, NC6, NE6, DH, ND6, NRS, NB5, DQ,
     NGS5, DE, NA5, NB5, DQ, NA5, DE, NGS5, NE5, DQ, NE5, DE, NA5, NC6, DQ, NB5, DE,
     NA5, NGS5, NE5, NGS5, NA5, NB5, DQ, NC6, NA5, NE5, NE5, DH, NRS, DE, NF5, DQ,
     NA5, DE, NC6, NC6, DS, NC6, NB5, DE, NA5, NG5, DDQ, NE5, DE, NG5, NA5, DS,
     NG5, NF5, DE, NE5, NGS5, NE5, NGS5, NA5, NB5, NGS5, NC6, NGS5, NA5, NE5, NE5, DQ,
     NE5, END
};

static const uint8_t music_Katyusha_A[] PROGMEM =
{
     41, NGS4, DDQ, NAS4, DE, NC5, DH, ND5, DDQ, NF5, DE, NE5, DQ, NC5, NAS4, DDQ, NC5, DE,
     ND5, DH, NE5, NF5, DDQ, NRS, DE, NGS5, DQ, NCS6, NC6, NCS6, DE, NC6, NAS5,
     NAS5, NGS5, NE5, NF5, DQ, NGS4, NRS, DE, NAS5, DQ, NG5, DE, NGS5, DDQ, NF5, DE, NG5,
     NC5, ND5, NE5, NF5, DDQ, NRS, DE, NGS5, DQ, NCS6, NC6, NCS6, DE, NC6, NAS5, NAS5,
     NGS5, NE5, NF5, DQ, NGS4, NRS, DE, NAS5, DQ, NG5, DE, NGS5, DDQ, NF5, DE, NG5,
     NC5, ND5, NE5, NF5, DDQ, NRS, DE, NGS4, DDQ, NAS4, DE, NC5, DH, ND5, NE5, DQ, NC5,
     NAS4, DDQ, NC5, DE, ND5, DH, NE5, NF5, DDQ, NRS, DE, NGS5, DQ, NCS6, NC6, NCS6, DE,
     NC6, NAS5, NAS5, NGS5, NE5, NF5, DQ, NGS4, NRS, DE, NAS5, DQ, NG5, DE, NGS5, DDQ,
     NF5, DE, NG5, NC5, ND5, NE5, NF5, DDQ, NRS, DE, NGS5, DQ, NCS6, NC6, NCS6, DE,
     NC6, NAS5, NAS5, NGS5, NE5, NF5, DQ, NGS4, NRS, DE, NAS5, DQ, NG5, DE, NGS5, DDQ,
     NF5, DE, NG5, NC5, ND5, NE5, NF5, DDQ, END
};

static const uint8_t music_Katyusha_B[] PROGMEM =
{
     41, NF5, DDQ, NG5, DE, NGS5, DDQ, NF5, DE, NGS5, NGS5, NG5, NRS, NG5, DQ, NRS, NG5, DDQ,
     NGS5, DE, NAS5, DDQ, NG5, DE, NAS5, NAS5, NGS5, NG5, NRS, DH, NC6, DQ, NF6,
     NDS6, NF6, DE, NDS6, NCS6, NCS6, NC6, NAS5, NC6, DQ, NF5, NRS, DE, NCS6, DQ, NAS5, DE,
     NC6, DDQ, NGS5, DE, NAS5, NAS5, NGS5, NG5, NRS, DH, NC6, DQ, NF6, NDS6, NF6, DE,
     NDS6, NCS6, NCS6, NC6, NAS5, NC6, DQ, NF5, NRS, DE, NCS6, DQ, NAS5, DE, NC6, DDQ,
     NGS5, DE, NAS5, NAS5, NGS5, NG5, NRS, DH, NF5, DDQ, NG5, DE, NGS5, DDQ, NF5, DE,
     NGS5, NGS5, NG5, NF5, NG5, DQ, NRS, NG5, DDQ, NGS5, DE, NAS5, DDQ, NG5, DE, NAS5,
     NAS5, NGS5, NG5, NRS, DH, NC6, DQ, NF6, NDS6, NF6, DE, NDS6, NCS6, NCS6, NC6, NAS5,
     NC6, DQ, NF5, NRS, DE, NCS6, DQ, NAS5, DE, NC6, DDQ, NGS5, DE, NAS5, NAS5, NGS5,
     NG5, NRS, DH, NC6, DQ, NF6, NDS6, NF6, DE, NDS6, NCS6, NCS6, NC6, NAS5, NC6, DQ, NF5,
     NRS, DE, NCS6, DQ, NAS5, DE, NC6, DDQ, NGS5, DE, NAS5, NAS5, NGS5, NG5, END
};

static const uint8_t music_HTTG_A[] PROGMEM =
{
     36, NG2, DE, NG3, NAS3, ND4, NG4, NAS4, NF5, NE5, DQ, NG3, DE, NC4, NE4, NG4, NC5,
     ND5, ND5, NA2, NE3, NA3, NC4, NE4, NA4, NE5, NE5, ND3, NA3, ND4, NF4, NA4, NF4,
     ND4, NA3, NG2, NG3, NAS3, ND4, NG4, NAS4, NG5, NG5, NA2, NE3, NA3, NC4, NE4, NC5,
     NG5, NG5, ND4, NC4, NCS4, ND4, NRS, ND3, NE3, NF3, DQ, NRS, DE, NF6, NC5, NG4,
     NE4, NC4, NG3, NE3, NC3, NG2, NC2, NG2, NG3, NAS3, ND4, NG4, NAS4, NF6, NE6, DQ, NG3, DE,
     NC4, NE4, NG4, NC5, ND6, ND6, NA2, NE3, NA3, NC4, NE4, NA4, NG6, NG6, ND3,
     NA3, ND4, NF4, NA4, NF4, ND4, NA3, NG2, NG3, NAS3, ND4, NG4, NAS4, ND6, NF6, NC3,
     NG3, NC4, NE4, NG4, NC6, NE6, NF6, DH, ND4, DE, NA3, NE3, NE3, ND3, DQ, ND3, DE,
     ND3, DQ, ND4, DE, NF6, NC4, NAS3, NAS3, ND4, NF4, ND4, NF3, NAS2, NF3, DQ, NC3, DE,
     NG3, NC4, NE4, NG4, NC5, NG4, NE4, NA2, NE3, NA3, NC4, NE4, NA4, NG6, NG6,
     ND3, NA3, ND4, NF4, NA4, NF4, ND4, NA3, NG2, NG3, NAS3, ND4, NG4, NAS4, NA6, NG6, DQ,
     NE3, DE, NA3, NC4, NE4, NA4, NC6, NC6, NAS3, NAS3, NF4, NC4, DDQ, NC5, DE, ND4, DQ,
     ND3, DE, ND3, DQ, ND4, DE, NF6, NC4, NAS3, NAS3, ND4, NF4, ND4, NF3, NAS2,
     NF3, DQ, NC3, DE, NG3, NC4, NE4, NG4, NC5, NG4, NE4, NA2, NE3, NA3, NC4, NE4,
     NA4, NG6, NG6, ND3, NA3, ND4, NF4, NA4, NF4, ND4, NA3, NG3, NG3, NAS3, NG3, NAS3,
     NG3, NAS3, NG3, NA3, NA3, NE4, NA3, NE4, NA3, NE4, NAS3, DDQ, NAS3, DE, NA3, DQ, NA3, DE,
     NC4, NAS3, DQ, NAS3, NC4, DDQ, NC4, DQ, NA3, NG4, DE, NF4, NE4, ND4, NC4,
     NB3, NA3, NA3, NC5, NG3, DQ, NG3, DE, NB4, DQ, NF3, DE, NF3, NF4, NF4, NC5,
     NF4, NF3, NC4, ND3, ND4, ND5, NG3, DQ, NG4, NC4, DDQ, NG4, DQ, NE4, NE3, NA3, DE,
     NA3, NC5, NG3, DQ, NG3, DE, NB4, DQ, NF3, DE, NF3, NF4, NF4, NC5, NF4,
     NF4, NC4, ND3, ND4, ND5, NG3, DQ, NG4, NA3, NA4, DE, NG4, NF4, NE4, ND4, NC4,
     NB3, NA3, NA3, NC5, NG3, DQ, NG3, DE, NB4, DQ, NF3, DE, NF3, NF4, NF4, NC5,
     NF4, NF4, NC4, ND3, ND4, ND5, NG3, DQ, NG4, NC4, DDQ, NC5, DE, NG4, NE4, NB3,
     NE3, NB2, NA3, NA3, NC5, NG3, DQ, NG3, DE, NB4, DQ, NF3, DE, NF3, NF4, NF4,
     NC5, NF4, NF3, NC4, ND3, ND4, ND5, NG3, DQ, NG4, NA3, NA3, DE, NA4, NA3, NB4,
     NA3, NC5, NA3, NA3, NE3, NA3, NG3, NRS, NG3, NRS, NF3, DW, NF3, DE, END
};

static const uint8_t music_HTTG_B[] PROGMEM =
{
     36, NA5, DQ, NF5, DE, NF5, DQ, ND5, DE, NRS, DQ, NC3, DE, NF5, NG5, DH, NRS, DQ,
     NE5, NC5, DE, NG5, DQ, NC5, DE, NRS, DQ, ND5, DH, ND5, DQ, NF5, NG5,
     NA5, DE, NG5, DDQ, NRS, DQ, NA5, NE5, DE, NE5, DQ, NRS, DDQ, NF5, DE, NC6,
     NCS6, ND6, NRS, NA6, NG6, NF6, DQ, NRS, NG6, NC6, DE, NC6, DDQ, NRS, NA6, DQ, NF6, DE,
     NF6, DQ, ND6, DE, NRS, DQ, NC3, DE, NF6, NG6, DH, NRS, DQ, NE6, NC6, DE,
     NG6, DQ, NA6, DE, NRS, DQ, NF6, DH, ND6, DQ, NA6, NAS6, NF6, DE, NF6, DDQ,
     NRS, DQ, NE6, NF6, DE, NG6, DQ, NRS, DDQ, ND3, DE, NA3, ND3, NF6, NG6,
     NE6, NC6, ND6, DH, ND6, DE, NA6, NRS, NG6, NF6, DDQ, ND6, DQ, NAS6, DE, NA6, DQ,
     NG6, NF6, DE, NG6, NG6, NE6, DDQ, NC6, DE, NC6, DQ, NAS6, DE, NA6, DQ, NE6, DE,
     NRS, DQ, NF6, DH, NRS, DE, NA6, NG6, NF6, DQ, ND6, DE, ND6, DDQ, ND6, DE,
     NRS, DQ, NA2, DE, NA6, NE6, DH, NRS, DQ, NA6, NA6, DE, NG6, DQ, NC6, DE,
     NC6, ND6, DH, ND6, DE, NA6, NRS, NG6, NF6, DDQ, ND6, DQ, NAS6, DE, NA6, DQ,
     NG6, NF6, DE, NG6, NG6, NE6, DDQ, NC6, DE, NC6, DQ, NAS6, DE, NA6, DQ, NE6, DE,
     NRS, DQ, NF6, DH, NRS, DE, ND6, NE6, NF6, DDQ, NG6, DE, NF6, DQ, NE6, DE,
     NF6, NG6, DDQ, NA6, DE, NG6, DDQ, NAS6, DE, NA6, DQ, NG6, DE, NA6, NAS6, DH,
     NAS6, DQ, NC7, NA6, NG6, DE, NA6, DH, NA6, DE, NA6, NGS6, NA6, NB6, DQ, NC7, DE,
     NB6, NG6, NG6, DQ, NE6, DE, NB6, NB6, NA6, DH, NRS, DE, NE6, NG6, NF6,
     ND6, DQ, NA6, DE, NG6, DQ, ND6, ND6, NE6, DE, NE6, DH, NRS, DE, NE6, NC7,
     NB6, ND7, NC7, DQ, NG6, DE, NB6, NB6, NA6, DH, NRS, DE, NA6, NG6, NA6, NF6, DQ,
     NA6, DE, NB6, DQ, NG6, NA6, DDQ, NA6, DE, NA6, NGS6, NA6, NB6, DQ, NC7, DE,
     NB6, NG6, NG6, DQ, NE6, DE, NB6, NB6, NA6, DH, NRS, DE, NE6, NG6, NF6,
     ND6, DQ, NA6, DE, NB6, DQ, NG6, NB6, NC7, DE, NB6, DH, NRS, DE, NE6, NC7,
     NB6, ND7, NC7, DQ, NE6, DE, NB6, NC7, DDH, NRS, DE, NC7, NF4, NA6, NF6, DQ, NA6, DE,
     ND7, DQ, NG6, NG6, NB6, DE, NA6, DDH, NC6, DE, NRS, NC6, ND6, NRS, ND6,
     NRS, NE6, DW, NE6, DE, END
};

static const uint8_t* const music_list[][2] PROGMEM =
{
    {music_alarm_beep, music_alarm_beep},
    {music_alarm_pulse, music_alarm_pulse},
    {music_Tetris_A_Theme_A, music_Tetris_A_Theme_B},
    {music_Katyusha_A, music_Katyusha_B},
    {music_HTTG_A, music_HTTG_B},
};

const uint8_t INBUILT_SONG_COUNT = (sizeof(music_list) / sizeof(music_list[0]));

uint8_t* GetMusicDATA(const uint8_t index, const uint8_t channel);
void PlayMusic(const uint8_t index);

#endif
