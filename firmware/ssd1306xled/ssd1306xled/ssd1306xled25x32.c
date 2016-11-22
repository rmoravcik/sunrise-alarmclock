/*
 *  sunrise-alarmclock
 *  Copyright (C) 2016 Roman Moravcik
 *
 *  Based on ssd1306xled8x16.c
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

#include <stdlib.h>
#include <avr/io.h>

#include <avr/pgmspace.h>

#include "ssd1306xled.h"
#include "ssd1306xled25x32.h"
#include "font25x32.h"

void ssd1306_string_font25x32xy(uint8_t x, uint8_t y, const char s[]) {
	uint16_t ch, k = 0;
	while (s[k] != '\0') {
		ch = s[k] - 48;
		if (x > 103) {
			x = 0;
		}
		ssd1306_setpos(x, y);
		for (uint8_t i = 0; i < 25; i++) {
			ssd1306_send_data_start();
			for (uint8_t j = 0; j < 4; j++) {
				ssd1306_send_byte(pgm_read_byte(&ssd1306xled_font25x32[100 * ch + 4 * i + j]));
			}
			ssd1306_send_data_stop();
		}
		x += 25;
		k++;
	}
}
