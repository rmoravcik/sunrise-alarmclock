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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "ds_rtc_lib/library-gcc/twi.h"
#include "ds_rtc_lib/library-gcc/test/uart.h"

#include "ssd1306xled/ssd1306xled/ssd1306xled.h"
#include "ssd1306xled/ssd1306xled/ssd1306xled25x32.h"

#include "common.h"
#include "audio.h"
#include "command.h"
#include "led.h"
#include "rtc_wrapper.h"

#ifdef DEBUG
uint8_t debug;
#endif

static conf_t EEMEM eeprom_conf;
conf_t conf;

volatile uint16_t status = 0;
volatile uint16_t period = 0;
volatile uint8_t display_period = 0;
volatile uint8_t wday = 0;
volatile uint8_t sec = 0;

struct tm set_time;

static inline uint8_t NEXT_WDAY(uint8_t current)
{
    uint8_t next = current + 1;

    if (next > 7)
        next = 1;

    return next;
}

static void calc_prealarm_time(uint8_t hour, uint8_t min,
                               uint8_t *prealarm_hour,
                               uint8_t *prealarm_min)
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
    uint8_t press_counter = 0;

    while (!(PIND & _BV(PD3)) && press_counter < 10) {
        _delay_ms(200);
        press_counter++;
    }

    // short press of func button
    if (press_counter == 1) {
        if (status & (PREALARM_RUNNING | ALARM_RUNNING)) {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("INT1_vect(): Reqest to stop PREALARM/ALARM\r\n");
            }
#endif
            status |= ALARM_STOP_REQUEST;
        } else {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("INT1_vect(): Display on\r\n");
            }
#endif
            status |= DISPLAY_ON;
        }
    }
}

ISR(PCINT1_vect, ISR_NOBLOCK)
{
    struct tm* time = NULL;
    char buf[50];

    time = rtc_wrapper_get_time();

    // PC2 is pin change interrupt, so 1Hz SQW signal
    // from DS1307 is triggering this interrupt every 500ms
    if (sec != time->sec) {
        sec = time->sec;

        if (status & (DISPLAY_ON | PREALARM_RUNNING | PREALARM_STOPPING |
                                   ALARM_RUNNING | ALARM_STOPPING)) {
            sprintf(buf, "%02d:%02d", time->hour, time->min);
            ssd1306_string_font25x32xy(3, 0, buf);

            display_period++;

            if ((status & DISPLAY_ON) && (display_period > DISPLAY_ON_SEC)) {
                ssd1306_clear();
                status &= ~DISPLAY_ON;
            }
        } else {
            display_period = 0;
        }

#ifdef DEBUG
        if (debug & DEBUG_RTC) {
            sprintf(buf, "Time: %02d:%02d:%02d (%d) (%04x)\r\n",
                    time->hour, time->min, time->sec, time->wday, status);
            uartSendString(buf);
        }
#endif
        if ((time->hour == 23) && (time->min == (60 - PREALARM_RUNNING_MIN)) &&
            (time->sec == 0)) {
            uint8_t next_wday = NEXT_WDAY(time->wday);

            if ((conf.alarm[WDAY_TO_ID(next_wday)].hour != ALARM_OFF_HOUR) &&
                (conf.alarm[WDAY_TO_ID(next_wday)].min != ALARM_OFF_MIN)) {
                uint8_t prealarm_hour, prealarm_min;

                calc_prealarm_time(conf.alarm[WDAY_TO_ID(next_wday)].hour,
                                   conf.alarm[WDAY_TO_ID(next_wday)].min,
                                   &prealarm_hour, &prealarm_min);

                rtc_wrapper_set_prealarm(prealarm_hour,
                                         prealarm_min);
            }
        }

        if (time->wday != wday) {
            if ((conf.alarm[WDAY_TO_ID(time->wday)].hour != ALARM_OFF_HOUR) &&
                (conf.alarm[WDAY_TO_ID(time->wday)].min != ALARM_OFF_MIN)) {
                rtc_wrapper_set_alarm(conf.alarm[WDAY_TO_ID(time->wday)].hour,
                                      conf.alarm[WDAY_TO_ID(time->wday)].min);
            }

            wday = time->wday;
        }
        if (rtc_wrapper_check_prealarm(time)) {
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

        if (rtc_wrapper_check_alarm(time)) {
            // Check if prealarm was previously running
            if (status & PREALARM_RUNNING) {
                status |= PREALARM_WAS_RUNNING;
            } else {
                status &= ~PREALARM_WAS_RUNNING;
            }

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

                // Start playback
                audio_set_volume(AUDIO_VOLUME_5);
                audio_play_alarm();
            }
        }
    }

    if (status & ALARM_STOP_REQUEST) {
        if (audio_is_playing()) {
            audio_stop_playback();
        }

        if (status & PREALARM_RUNNING) {
            status &= ~(PREALARM_RUNNING | ALARM_STOP_REQUEST);
            status |= PREALARM_STOPPING;
            period = 0;
        } else if (status & ALARM_RUNNING) {
            status &= ~(ALARM_RUNNING | ALARM_STOP_REQUEST);
            status |= ALARM_STOPPING;
            period = 0;
        }
    }

    if (status & PREALARM_RUNNING) {
        led_sunrise(period);
        period++;
    } else if (status & ALARM_RUNNING) {
        if (period >= (SEC_TO_PERIOD(ALARM_RUNNING_SEC))) {
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
            if (period == SEC_TO_PERIOD(ALARM_RUNNING_SEC / 2)) {
                // Start playback again
                audio_set_volume(AUDIO_VOLUME_6);
                audio_play_alarm();
            }

            if (status & PREALARM_WAS_RUNNING) {
                led_on();
            }
            period++;
        }
    }

    if (status & PREALARM_STOPPING) {
        if (period >= (TO_PERIOD(PREALARM_STOPPING_MIN))) {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("PCINT1_vect(): PREALARM stopped\r\n");
            }
#endif
            ssd1306_clear();
            status &= ~PREALARM_STOPPING;
        }

        led_sunset(period);
        period++;
    } else if (status & ALARM_STOPPING) {
        if (period >= (TO_PERIOD(ALARM_STOPPING_MIN))) {
#ifdef DEBUG
            if (debug & DEBUG_FSM) {
                uartSendString("PCINT1_vect(): ALARM stopped\r\n");
            }
#endif
            ssd1306_clear();
            status &= ~(ALARM_STOPPING | PREALARM_WAS_RUNNING);
        }

        if (status & PREALARM_WAS_RUNNING) {
            led_sunset(period);
        }
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

void alarm_init(void)
{
    struct tm* time = NULL;

    time = rtc_wrapper_get_time();

    wday = time->wday;

#ifdef DEBUG
    uartSendString("alarm_init()\r\n");
#endif

    // Check if ALARM for current day is set and set it
    if ((conf.alarm[WDAY_TO_ID(wday)].hour != ALARM_OFF_HOUR) &&
        (conf.alarm[WDAY_TO_ID(wday)].min != ALARM_OFF_MIN)) {
        rtc_wrapper_set_alarm(conf.alarm[WDAY_TO_ID(wday)].hour,
                              conf.alarm[WDAY_TO_ID(wday)].min);

        if ((conf.alarm[WDAY_TO_ID(wday)].hour == 0) &&
            (conf.alarm[WDAY_TO_ID(wday)].min < PREALARM_RUNNING_MIN)) {
            // Do not set PREALARM if ALARM time is < 00:20
            // PREALARM for these times is 23:40 - 23:59
        } else {
            if ((time->hour == 23) &&
                (time->min >= (60 - PREALARM_RUNNING_MIN))) {
                // Do not set PREALARM if current time is >= 23:40
            } else {
                uint8_t prealarm_hour, prealarm_min;

                calc_prealarm_time(conf.alarm[WDAY_TO_ID(wday)].hour,
                                   conf.alarm[WDAY_TO_ID(wday)].min,
                                   &prealarm_hour, &prealarm_min);
                rtc_wrapper_set_prealarm(prealarm_hour,
                                         prealarm_min);
            }
        }
    } else {
        // If ALARM is disabled, reset both alarms
        rtc_wrapper_reset_alarm();
        rtc_wrapper_reset_prealarm();
    }

    // If current time is >= 23:40, check if ALARM for the next day is set
    // and set PREALARM
    if ((time->hour == 23) &&
        (time->min >= (60 - PREALARM_RUNNING_MIN))) {
        uint8_t next_wday = NEXT_WDAY(wday);

        if ((conf.alarm[WDAY_TO_ID(next_wday)].hour != ALARM_OFF_HOUR) &&
            (conf.alarm[WDAY_TO_ID(next_wday)].min != ALARM_OFF_MIN)) {
            uint8_t prealarm_hour, prealarm_min;

            calc_prealarm_time(conf.alarm[WDAY_TO_ID(next_wday)].hour,
                               conf.alarm[WDAY_TO_ID(next_wday)].min,
                               &prealarm_hour, &prealarm_min);
            rtc_wrapper_set_prealarm(prealarm_hour,
                                     prealarm_min);
        }
    }
}

int main(void)
{
#ifdef DEBUG
    debug = DEBUG_RTC | DEBUG_LED | DEBUG_FSM | DEBUG_COMMAND;
    char buf[50];
#endif

    OSCCAL = 0x8f;

    eeprom_init();

    uartInit();
    uartSetBaudRate(38400);
    uartFlushReceiveBuffer();
    uartSetRxHandler(&command_rx_handler);

    twi_init_master();

    rtc_wrapper_init();

    ssd1306_init();

    led_init();

    snooze_irq_init();

    alarm_init();

    audio_init();

    sei();

    ssd1306_clear();

    status |= DISPLAY_ON;

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
                struct tm* time = NULL;

                time = rtc_wrapper_get_time();


                if ((conf.alarm[id].hour != ALARM_OFF_HOUR) &&
                    (conf.alarm[id].min != ALARM_OFF_MIN)) {
                    // Set ALARM always
                    rtc_wrapper_set_alarm(conf.alarm[id].hour,
                                          conf.alarm[id].min);

                    if ((conf.alarm[id].hour == 0) &&
                        (conf.alarm[id].min < PREALARM_RUNNING_MIN)) {
                        // Do not set PREALARM if ALARM time is < 00:20
                        // PREALARM for these times is 23:40 - 23:59
                    } else {
                        if ((time->hour == 23) &&
                            (time->min >= (60 - PREALARM_RUNNING_MIN))) {
                            // Do not set PREALARM if current time is >= 23:40
                        } else {
                            uint8_t prealarm_hour, prealarm_min;

                            calc_prealarm_time(conf.alarm[id].hour,
                                               conf.alarm[id].min,
                                               &prealarm_hour, &prealarm_min);
                            rtc_wrapper_set_prealarm(prealarm_hour,
                                                     prealarm_min);
                        }
                    }
                } else {
                    // Reset ALARM always
                    rtc_wrapper_reset_alarm();

                    if ((time->hour == 23) &&
                        (time->min >= (60 - PREALARM_RUNNING_MIN))) {
                        // Do not reset PREALARM if current time is > 23:40,
                        // because PREALARM for next day alarm should be
                        // already set
                    } else {
                        rtc_wrapper_reset_prealarm();
                    }
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

            rtc_wrapper_set_time(&set_time);
            status &= ~SET_DATE;
        }

        _delay_ms(500);
    }
}
