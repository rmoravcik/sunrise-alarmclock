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
#include "ds_rtc_lib/library-gcc/test/uart.h"

#include "ssd1306xled/ssd1306xled/ssd1306xled.h"

#include "command.h"
#include "common.h"
#include "led.h"
#include "rtc2.h"

static conf_t EEMEM eeprom_conf;
conf_t conf;

uint16_t status = 0;
uint8_t wday = 0;

struct tm set_time;

ISR(INT1_vect, ISR_NOBLOCK)
{
    uartSendString("Snooze!\r\n");
    status |= PREALARM;
}

ISR(PCINT1_vect, ISR_NOBLOCK)
{
    struct tm* time = NULL;
    static uint16_t period = 0;
#ifdef DEBUG
    char buf[50];
#endif

    time = rtc2_get_time();
#ifdef DEBUG
    sprintf(buf, "Time: %02d:%02d:%02d\r\n", time->hour, time->min, time->sec);
    uartSendString(buf);
#endif

    if ((time->hour == 23) && (time->min == (60 - PREALARM_MIN)) &&
        (time->sec == 0)) {
        if ((conf.alarm[WDAY_TO_ID(time->wday + 1)].hour != ALARM_OFF_HOUR) &&
            (conf.alarm[WDAY_TO_ID(time->wday + 1)].min != ALARM_OFF_MIN)) {
        }
    }

    if (time->wday != wday) {
        if ((conf.alarm[WDAY_TO_ID(time->wday)].hour != ALARM_OFF_HOUR) &&
            (conf.alarm[WDAY_TO_ID(time->wday)].min != ALARM_OFF_MIN)) {
            rtc2_set_alarm(conf.alarm[WDAY_TO_ID(time->wday)].hour,
                            conf.alarm[WDAY_TO_ID(time->wday)].min);

#ifdef DEBUG
            sprintf(buf, "SET ALARM: %02d:%02d:%02d\r\n",
                    conf.alarm[time->wday - 1].hour,
                    conf.alarm[time->wday - 1].min,
                    0);
            uartSendString(buf);
#endif
        }

        wday = time->wday;
    }

    if (rtc2_check_prealarm(time)) {
#ifdef DEBUG
        uartSendString("PREALARM!\r\n");
#endif
        // FIXME
    }

    if (rtc2_check_alarm(time)) {
#ifdef DEBUG
        uartSendString("ALARM!\r\n");
#endif
        // FIXME
    }

    if (status & PREALARM) {
        led_sunrise(period);
        period++;
    }
}

void snooze_irq_init(void)
{
    DDRD &= ~_BV(PD3);
    PORTD |= _BV(PD3);

    EICRA |= _BV(ISC11);
    EIMSK |= _BV(INT1);
}

void eeprom_init(void)
{
    uint8_t id = 0;

    eeprom_read_block(&conf, &eeprom_conf, sizeof(conf_t));

    for (id = 0; id < 7; id++) {
        if ((conf.alarm[id].hour == 0xff) &&
            (conf.alarm[id].min == 0xff)) {
            conf.alarm[id].hour = ALARM_OFF_HOUR;
            conf.alarm[id].min = ALARM_OFF_MIN;
        }
    }

    eeprom_write_block(&conf, &eeprom_conf, sizeof(conf_t));
}

int main(void)
{
#ifdef DEBUG
    char buf[50];
#endif

    OSCCAL = 0x8f;

    uartInit();
    uartSetBaudRate(38400);

    uartFlushReceiveBuffer();
    uartSetRxHandler(&command_rx_handler);

    twi_init_master();

    rtc2_init();

    ssd1306_init();

    led_init();

    snooze_irq_init();

    eeprom_init();

    sei();

    while (1) {
        if (status & SET_ALARM) {
            uint8_t id = status & SET_ALARM_ID_MASK;

#ifdef DEBUG
            sprintf(buf, "SET ALARM%d: Time %02u:%02u\r\n",
                    id, set_time.hour, set_time.min);
            uartSendString(buf);
#endif

            conf.alarm[id].hour = set_time.hour;
            conf.alarm[id].min = set_time.min;

            eeprom_write_block(&conf, &eeprom_conf, sizeof(conf_t));

            if (id == WDAY_TO_ID(wday)) {
                if ((conf.alarm[id].hour != ALARM_OFF_HOUR) &&
                    (conf.alarm[id].min != ALARM_OFF_MIN)) {
                    uint8_t prealarm_hour, prealarm_min;

                    if (conf.alarm[id].min < PREALARM_MIN) {
                        prealarm_min = 60 - PREALARM_MIN + conf.alarm[id].min;

                        if (conf.alarm[id].hour == 0) {
                            prealarm_hour = 23;
                        } else {
                            prealarm_hour = conf.alarm[id].hour - 1;
                        }
                    } else {
                        prealarm_min = conf.alarm[id].min - PREALARM_MIN;
                        prealarm_hour = conf.alarm[id].hour;
                    }

#ifdef DEBUG
                    sprintf(buf, "SET PREALARM%d: Time %02u:%02u\r\n",
                            id, prealarm_hour, prealarm_min);
                    uartSendString(buf);
#endif
                    rtc2_set_prealarm(prealarm_hour,
                                      prealarm_min);
                    rtc2_set_alarm(conf.alarm[id].hour,
                                   conf.alarm[id].min);
                } else {
                    rtc2_reset_prealarm();
                    rtc2_reset_alarm();
                }
            }

            status &= ~(SET_ALARM | SET_ALARM_ID_MASK);
        }

        if (status & SET_DATE) {
#ifdef DEBUG
            sprintf(buf, "SET DATE: Time %02u:%02u:%02u Date %02u:%02u:%04u (%1u)\r\n",
                    set_time.hour, set_time.min, set_time.sec, set_time.mday,
                    set_time.mon, set_time.year, set_time.wday);
            uartSendString(buf);
#endif

            rtc2_set_time(&set_time);
            status &= ~SET_DATE;
        }

        _delay_ms(500);
    }
}
