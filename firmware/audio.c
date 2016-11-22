/*
 *  sunrise-alarmclock
 *  Copyright (C) 2016 Roman Moravcik
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>

#include <avr/eeprom.h>
#include <util/delay.h>

#include "common.h"
#include "audio.h"

#define PLAY_1ST_VOICE		0x0000
#define VOLUME_ADJ		0xFFF0
#define STOP_PLAYBACK		0xFFFF

#define GPIO_WTV020_DATA	PB0
#define GPIO_WTV020_CLK		PB1
#define GPIO_WTV020_RESET	PB2

static uint8_t volume = AUDIO_VOLUME_7;

static bool is_playing = false;

static void wtv020_reset(void)
{
    PORTB &= ~_BV(GPIO_WTV020_RESET);
    _delay_ms(5);
    PORTB |= _BV(GPIO_WTV020_RESET);
    _delay_ms(300);
}

static void wtv020_send_command(uint16_t command)
{
    uint8_t i;

    PORTB &= ~_BV(GPIO_WTV020_CLK);
    _delay_us(1950);

    for (i = 0; i < 16; i++) {
        PORTB &= ~_BV(GPIO_WTV020_CLK);
        _delay_us(50);

        if (command & 0x0001) {
            PORTB |= _BV(GPIO_WTV020_DATA);
        } else {
            PORTB &= ~_BV(GPIO_WTV020_DATA);
        }

        _delay_us(50);

        PORTB |= _BV(GPIO_WTV020_CLK);

        _delay_us(100);

        command >>= 1;
    }
    _delay_us(1900);
}

void audio_init(void)
{
    // Set DATA and RESET pins as an outputs
    DDRB |= _BV(GPIO_WTV020_DATA) | _BV(GPIO_WTV020_CLK) | _BV(GPIO_WTV020_RESET);

    // Set DATA and RESET pins to high
    PORTD |= _BV(GPIO_WTV020_DATA) | _BV(GPIO_WTV020_CLK) | _BV(GPIO_WTV020_RESET);

    _delay_ms(100);

    wtv020_reset();

    wtv020_send_command(volume);
}

bool audio_is_playing(void)
{
    return is_playing;
}

void audio_play_alarm(void)
{
    wtv020_send_command(PLAY_1ST_VOICE);
    is_playing = true;
}

void audio_set_volume(uint8_t level)
{
    if (level > AUDIO_VOLUME_7)
        level = AUDIO_VOLUME_7;

    wtv020_send_command(VOLUME_ADJ + level);
}

uint8_t audio_get_volume(void)
{
    return volume;
}

void audio_stop_playback(void)
{
    is_playing = false;
    wtv020_send_command(STOP_PLAYBACK);
}
