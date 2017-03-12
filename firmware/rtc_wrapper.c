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

#include "ds_rtc_lib/library-gcc/test/uart.h"

#include "common.h"
#include "rtc_wrapper.h"

static void rtc_SQW_irq_init(void)
{
    DDRC &= ~_BV(PC2);

    PCMSK1 |= _BV(PCINT10);
    PCICR |= _BV(PCIE1);
}

void rtc_wrapper_init(void)
{
    rtc_init();

    rtc_SQW_irq_init();
    rtc_SQW_enable(true);
    rtc_SQW_set_freq(FREQ_1);
}

struct tm* rtc_wrapper_get_time(void)
{
    return rtc_get_time();
}

void rtc_wrapper_set_time(struct tm* tm_)
{
    rtc_set_time(tm_);
}

void rtc_wrapper_reset_prealarm(void)
{
#ifdef DEBUG
    if (debug & DEBUG_RTC) {
        uartSendString("rtc_wrapper_reset_prealarm()\r\n");
    }
#endif

    rtc_set_sram_byte(ALARM_OFF_HOUR, 0); // hour
    rtc_set_sram_byte(ALARM_OFF_MIN, 1); // minute
}

void rtc_wrapper_set_prealarm(uint8_t hour, uint8_t min)
{
#ifdef DEBUG
    char buf[50];

    if (debug & DEBUG_RTC) {
        sprintf(buf, "rtc_wrapper_set_prealarm(): %02d:%02d\r\n", hour, min);
        uartSendString(buf);
    }
#endif

    rtc_set_sram_byte(hour, 0); // hour
    rtc_set_sram_byte(min,  1); // minute
}

void rtc_wrapper_get_prealarm(uint8_t* hour, uint8_t* min)
{
    if (hour) *hour = rtc_get_sram_byte(0);
    if (min)  *min  = rtc_get_sram_byte(1);
}

bool rtc_wrapper_check_prealarm(struct tm *tm_)
{
    uint8_t hour = rtc_get_sram_byte(0);
    uint8_t min  = rtc_get_sram_byte(1);

    if (tm_->hour == hour && tm_->min == min && tm_->sec == 0) {
#ifdef DEBUG
        if (debug & DEBUG_RTC) {
            uartSendString("rtc_wrapper_check_prealarm(): PREALARM!\r\n");
        }
#endif
        return true;
    }
    return false;
}

void rtc_wrapper_reset_alarm(void)
{
#ifdef DEBUG
    if (debug & DEBUG_RTC) {
        uartSendString("rtc_wrapper_reset_alarm()\r\n");
    }
#endif

    rtc_set_sram_byte(ALARM_OFF_HOUR, 2); // hour
    rtc_set_sram_byte(ALARM_OFF_MIN, 3); // minute
}

void rtc_wrapper_set_alarm(uint8_t hour, uint8_t min)
{
#ifdef DEBUG
    char buf[50];

    if (debug & DEBUG_RTC) {
        sprintf(buf, "rtc_wrapper_set_alarm(): %02d:%02d\r\n", hour, min);
        uartSendString(buf);
    }
#endif

    rtc_set_sram_byte(hour, 2); // hour
    rtc_set_sram_byte(min,  3); // minute
}

void rtc_wrapper_get_alarm(uint8_t* hour, uint8_t* min)
{
    if (hour) *hour = rtc_get_sram_byte(2);
    if (min)  *min  = rtc_get_sram_byte(3);
}

bool rtc_wrapper_check_alarm(struct tm *tm_)
{
    uint8_t hour = rtc_get_sram_byte(2);
    uint8_t min  = rtc_get_sram_byte(3);

    if (tm_->hour == hour && tm_->min == min && tm_->sec == 0) {
#ifdef DEBUG
        if (debug & DEBUG_RTC) {
            uartSendString("rtc_wrapper_check_alarm(): ALARM!\r\n");
        }
#endif
        return true;
    }
    return false;
}
