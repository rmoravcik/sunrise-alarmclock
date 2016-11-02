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

#ifdef DEBUG
uint8_t debug;
#endif

static conf_t EEMEM eeprom_conf;
conf_t conf;

volatile uint32_t status = 0;
volatile uint16_t period = 0;
volatile uint8_t wday = 0;

struct tm set_time;

void calc_prealarm_time(uint8_t hour, uint8_t min,
                        uint8_t *prealarm_hour, uint8_t *prealarm_min)
{
    if (min < PREALARM_RUNNING_MIN) {
        *prealarm_min = 60 - PREALARM_RUNNING_MIN + min;

        if (hour == 0) {
            *prealarm_hour = 23;
        } else {
            *prealarm_hour = hour - 1;
        }
    } else {
        *prealarm_min = min - PREALARM_RUNNING_MIN;
        *prealarm_hour = hour;
    }
}

ISR(INT1_vect, ISR_NOBLOCK)
{
    // REMOVEME: JUST FOR a TEST
    status |= PREALARM_RUNNING;

    if (status & (PREALARM_RUNNING | ALARM_RUNNING)) {
#ifdef DEBUG
        if (debug & DEBUG_FSM) {
            uartSendString("INT1_vect(): Reqest to stop PREALARM/ALARM\r\n");
        }
#endif
        status |= ALARM_STOP_REQUEST;
    } else {
        // FIXME: Just show time on display
    }
}

ISR(PCINT1_vect, ISR_NOBLOCK)
{
    struct tm* time = NULL;
#ifdef DEBUG
    char buf[50];
#endif

    time = rtc2_get_time();
#ifdef DEBUG
    if (debug & DEBUG_RTC) {
        sprintf(buf, "rtc2_get_time(): %02d:%02d:%02d\r\n", time->hour, time->min, time->sec);
        uartSendString(buf);
    }
#endif

    if ((time->hour == 23) && (time->min == (60 - PREALARM_RUNNING_MIN)) &&
        (time->sec == 0)) {
        if ((conf.alarm[WDAY_TO_ID(time->wday + 1)].hour != ALARM_OFF_HOUR) &&
            (conf.alarm[WDAY_TO_ID(time->wday + 1)].min != ALARM_OFF_MIN)) {
            uint8_t prealarm_hour, prealarm_min;

            calc_prealarm_time(conf.alarm[WDAY_TO_ID(time->wday + 1)].hour,
                               conf.alarm[WDAY_TO_ID(time->wday + 1)].min,
                               &prealarm_hour, &prealarm_min);

            rtc2_set_prealarm(prealarm_hour,
                              prealarm_min);
        }
    }

    if (time->wday != wday) {
        if ((conf.alarm[WDAY_TO_ID(time->wday)].hour != ALARM_OFF_HOUR) &&
            (conf.alarm[WDAY_TO_ID(time->wday)].min != ALARM_OFF_MIN)) {
            rtc2_set_alarm(conf.alarm[WDAY_TO_ID(time->wday)].hour,
                            conf.alarm[WDAY_TO_ID(time->wday)].min);
        }

        wday = time->wday;
    }

    if (rtc2_check_prealarm(time)) {
        if (status & (PREALARM_RUNNING | ALARM_RUNNING | ALARM_STOP_REQUEST)) {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("PCINT1_vect(): Unabled to start PREALARM\r\n");
            }
#endif
            // There is an ALARM already running, do nothing
        } else {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("PCINT1_vect(): Starting PREALARM\r\n");
            }
#endif
            // Start PREALARM
            status |= PREALARM_RUNNING;
            period = 0;
        }
    }

    if (rtc2_check_alarm(time)) {
        if (status & (ALARM_STOP_REQUEST)) {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("PCINT1_vect(): ALARM already stopped\r\n");
            }
#endif
            // ALARM was stoped already during PREALARM
            status &= ~ALARM_STOP_REQUEST;
        } else {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("PCINT1_vect(): Starting ALARM\r\n");
            }
#endif
            // Stop PREALARM if it was previously running
            // and start ALARM
            status &= ~PREALARM_RUNNING;
            status |= ALARM_RUNNING;
            period = 0;

            // FIXME: Start playback
        }
    }

    if (status & ALARM_STOP_REQUEST) {
        if (status & PREALARM_RUNNING) {
            status &= ~PREALARM_RUNNING;
            status |= PREALARM_STOPPING;
            period = 0;
        } else if (status & ALARM_RUNNING) {
            status &= ~ALARM_RUNNING;
            status |= ALARM_STOPPING;
            period = 0;
        }
    }

    if (status & PREALARM_RUNNING) {
        led_sunrise(period);
        period++;
    } else if (status & ALARM_RUNNING) {
        if (period >= (ALARM_RUNNING_MIN * 120)) {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("PCINT1_vect(): ALARM timeout\r\n");
            }
#endif
            // Timeout, ALARM was not stopped
            status &= ~ALARM_RUNNING;
            status |= ALARM_STOPPING;
            period = 0;
        } else {
            led_on();
            period++;
        }
    }

    if (status & PREALARM_STOPPING) {
        if (period >= (PREALARM_STOPPING_MIN * 120)) {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("PCINT1_vect(): PREALARM stopped\r\n");
            }
#endif
            status &= ~PREALARM_STOPPING;
        }

        led_sunset(period);
        period++;
    } else if (status & ALARM_STOPPING) {
        if (period >= (ALARM_STOPPING_MIN * 120)) {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("PCINT1_vect(): ALARM stopped\r\n");
            }
#endif
            status &= ~ALARM_STOPPING;
        }

        led_sunset(period);
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
    debug = DEBUG_OFF;
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
            if (debug & DEBUG_COMMAND) {
                sprintf(buf, "SET ALARM%d: Time %02u:%02u\r\n",
                        id, set_time.hour, set_time.min);
                uartSendString(buf);
            }
#endif

            conf.alarm[id].hour = set_time.hour;
            conf.alarm[id].min = set_time.min;

            eeprom_write_block(&conf, &eeprom_conf, sizeof(conf_t));

            if (id == WDAY_TO_ID(wday)) {
                if ((conf.alarm[id].hour != ALARM_OFF_HOUR) &&
                    (conf.alarm[id].min != ALARM_OFF_MIN)) {
                    uint8_t prealarm_hour, prealarm_min;

                    calc_prealarm_time(conf.alarm[id].hour,
                                       conf.alarm[id].min,
                                       &prealarm_hour, &prealarm_min);
#ifdef DEBUG
                    if (debug & DEBUG_COMMAND) {
                        sprintf(buf, "SET PREALARM%d: Time %02u:%02u\r\n",
                                id, prealarm_hour, prealarm_min);
                        uartSendString(buf);
                    }
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
            if (debug & DEBUG_COMMAND) {
                sprintf(buf, "SET DATE: Time %02u:%02u:%02u Date %02u:%02u:%04u (%1u)\r\n",
                        set_time.hour, set_time.min, set_time.sec, set_time.mday,
                        set_time.mon, set_time.year, set_time.wday);
                uartSendString(buf);
            }
#endif

            rtc2_set_time(&set_time);
            status &= ~SET_DATE;
        }

        _delay_ms(500);
    }
}
