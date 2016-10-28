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

#include "ssd1306xled/ssd1306xled/ssd1306xled.h"

#include "command.h"
#include "common.h"

static conf_t EEMEM eeprom_conf;
conf_t conf;

uint16_t status = 0;

struct tm set_time;

ISR(INT1_vect, ISR_NOBLOCK)
{
    uartSendString("Snooze!\r\n");
}

ISR(PCINT1_vect, ISR_NOBLOCK)
{
    uartSendString("Tick!\r\n");
}

void rtc_SQW_irq_init(void)
{
    DDRC &= ~_BV(PC2);

    PCMSK1 |= _BV(PCINT10);
    PCICR |= _BV(PCIE1);
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
    struct tm* time = NULL;
    uint8_t wday = 0;
#ifdef DEBUG
    char buf[50];
#endif

    uartInit();
    uartSetBaudRate(115200);
    uartSetRxHandler(&command_rx_handler);

    twi_init_master();

    rtc_init();

    rtc_SQW_irq_init();
    rtc_SQW_set_freq(FREQ_1);
    rtc_SQW_enable(true);

    snooze_irq_init();

    eeprom_init();

    sei();

    while (1) {
        time = rtc_get_time();

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
                    rtc_set_alarm_s(conf.alarm[id].hour,
                                    conf.alarm[id].min,
                                    0);
                } else {
                    rtc_reset_alarm();
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

            rtc_set_time(&set_time);
            status &= ~SET_DATE;
        }

#ifdef DEBUG
        sprintf(buf, "Time: %02d:%02d:%02d\r\n", time->hour, time->min, time->sec);
        uartSendString(buf);
#endif

        if (time->wday != wday) {
            if ((conf.alarm[WDAY_TO_ID(time->wday)].hour != ALARM_OFF_HOUR) &&
                (conf.alarm[WDAY_TO_ID(time->wday)].min != ALARM_OFF_MIN)) {
                rtc_set_alarm_s(conf.alarm[WDAY_TO_ID(time->wday)].hour,
                                conf.alarm[WDAY_TO_ID(time->wday)].min,
                                0);

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

        if (rtc_check_alarm()) {
#ifdef DEBUG
            uartSendString("ALARM!\r\n");
#endif
            // FIXME
        }

        _delay_ms(500);
    }
}
