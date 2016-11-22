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

#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

enum {
    AUDIO_VOLUME_0 = 0,
    AUDIO_VOLUME_1,
    AUDIO_VOLUME_2,
    AUDIO_VOLUME_3,
    AUDIO_VOLUME_4,
    AUDIO_VOLUME_5,
    AUDIO_VOLUME_6,
    AUDIO_VOLUME_7,
};

void audio_init(void);

bool audio_is_playing(void);

void audio_play_alarm(void);

void audio_set_volume(uint8_t level);
uint8_t audio_get_volume(void);

void audio_stop_playback(void);

#endif
