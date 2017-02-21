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
#include <util/delay.h>

#include "ds_rtc_lib/library-gcc/test/uart.h"

#include "common.h"
#include "light_sensor.h"

void light_sensor_init(void)
{
    DDRC &= ~_BV(PC1);
    ADMUX |= _BV(REFS0) | _BV(MUX0);
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
}

bool light_sensor_is_dark(void)
{
#ifdef DEBUG
    char buf[60];
#endif

    ADCSRA |= _BV(ADSC);

    while (ADCSRA & _BV(ADSC)) {
    }

#ifdef DEBUG
    if (debug & DEBUG_ADC) {
        sprintf(buf, "light_sensor_is_dark: adc=%4u\r\n", ADCW);
        uartSendString(buf);
    }
#endif
    if (ADCW > 900)
        return true;

    return false;
}
