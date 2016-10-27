/*
 * DS RTC Library: DS1307 and DS3231 driver library
 * (C) 2011 Akafugu Corporation
 *
 * This program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 */

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "ds_rtc_lib/library-gcc/twi.h"
#include "ds_rtc_lib/library-gcc/rtc.h"
#include "ds_rtc_lib/library-gcc/test/uart.h"

#include "common.h"

#define DEBUG 1

typedef struct {
    uint8_t hour;
    uint8_t min;
} alarm_time_t;

typedef struct {
    alarm_time_t alarm[7];
} conf_t;

static conf_t EEMEM eeprom_conf;
static conf_t conf;

void uart_rx_handler(unsigned char c)
{
}

int main(void)
{
    struct tm* time = NULL;
    uint8_t wday = 0;
#ifdef DEBUG
    char buf[32];
#endif

    uartInit();
    uartSetBaudRate(115200);
    uartSetRxHandler(&uart_rx_handler);

#ifdef DEBUG
    uartSendString("DS RTC Library Test\r\n");
#endif

    twi_init_master();
    rtc_init();

    eeprom_read_block(&conf, &eeprom_conf, sizeof(conf_t));

#if 0
    rtc_set_time_s(12, 0, 50);

    time.hour = 11;
    time.min = 24;
    time.sec = 0;

    time.mday = 23;
    time.mon = 10;
    time.year = 1917;

    rtc_set_time(&time);
#endif

    while (1) {
        time = rtc_get_time();

#ifdef DEBUG
        sprintf(buf, "Time: %02d:%02d:%02d\r\n", time->hour, time->min, time->sec);
        uartSendString(buf);
#endif

        if (time->wday != wday) {
            rtc_set_alarm_s(conf.alarm[time->wday - 1].hour,
                            conf.alarm[time->wday - 1].min,
                            0);

#ifdef DEBUG
            sprintf(buf, "Alarm is set: %02d:%02d:%02d\r\n",
                    conf.alarm[time->wday - 1].hour,
                    conf.alarm[time->wday - 1].min,
                    0);
            uartSendString(buf);
#endif

            wday = time->wday;
        }

        if (rtc_check_alarm()) {
#ifdef DEBUG
            uartSendString("ALARM!\r\n");
#endif
            // FIXME
        }

        _delay_ms(500);
    }
}
