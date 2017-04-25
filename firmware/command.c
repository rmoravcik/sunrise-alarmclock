/*
 *  sunrise-alarmclock
 *  Copyright (C) 2016,2017 Roman Moravcik
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
#include <string.h>

#include "ds_rtc_lib/library-gcc/rtc.h"
#include "ds_rtc_lib/library-gcc/test/uart.h"

#include "common.h"

#ifdef DEBUG
#include "led.h"
#endif
#include "command.h"

#define RXBUFF_LEN 30
#define TXBUFF_LEN 50

extern conf_t conf;
extern uint16_t status;

extern struct tm set_time;

static char rxbuff[RXBUFF_LEN];

static void command_parse(uint8_t len)
{
    char txbuff[TXBUFF_LEN];

    if (strncmp(rxbuff, COMMAND_PING, strlen(COMMAND_PING)) == 0) {
        sprintf(txbuff, "%s\n", COMMAND_PING_RSP);
        uartSendString(txbuff);
    } else if (strncmp(rxbuff, COMMAND_GET_STATUS, strlen(COMMAND_GET_STATUS)) == 0) {
        // STAT+HH:mm;HH:mm;HH:mm;HH:mm;HH:mm;HH:mm;HH:mm
        sprintf(txbuff, "%s%02u:%02u;%02u:%02u;%02u:%02u;%02u:%02u;" \
                        "%02u:%02u;%02u:%02u;%02u:%02u\n",
                        COMMAND_GET_STATUS_RSP,
                        conf.alarm[0].hour, conf.alarm[0].min,
                        conf.alarm[1].hour, conf.alarm[1].min,
                        conf.alarm[2].hour, conf.alarm[2].min,
                        conf.alarm[3].hour, conf.alarm[3].min,
                        conf.alarm[4].hour, conf.alarm[4].min,
                        conf.alarm[5].hour, conf.alarm[5].min,
                        conf.alarm[6].hour, conf.alarm[6].min);
        uartSendString(txbuff);
    } else if (strncmp(rxbuff, COMMAND_SET_ALARM, strlen(COMMAND_SET_ALARM)) == 0) {
        unsigned int id, hour, min;

        // ALARM+F;HH:mm
        sscanf(rxbuff, "ALARM+%1u;%2u:%2u", &id, &hour, &min);

        set_time.hour = hour;
        set_time.min = min;

        status |= SET_ALARM | id;

        sprintf(txbuff, "%s\n", COMMAND_SET_ALARM_RSP);
        uartSendString(txbuff);
    } else if (strncmp(rxbuff, COMMAND_SET_DATE, strlen(COMMAND_SET_DATE)) == 0) {
        unsigned int hour, min, sec, wday, mday, mon, year;

        // DATE+HH:mm:ss;F;dd.MM.yyyy
        sscanf(rxbuff, "DATE+%2u:%2u:%2u;%1u;%2u.%2u.%4u",
                        &hour, &min, &sec, &wday, &mday,
                        &mon, &year);

        set_time.hour = hour;
        set_time.min = min;
        set_time.sec = sec;

        set_time.mday = mday;
        set_time.mon = mon;
        set_time.year = year - 99;

        set_time.wday = wday;

        status |= SET_DATE;

        sprintf(txbuff, "%s\n", COMMAND_SET_DATE_RSP);
        uartSendString(txbuff);
#ifdef DEBUG
    } else if (strncmp(rxbuff, COMMAND_SET_LED, strlen(COMMAND_SET_LED)) == 0) {
        unsigned int red, green, blue;

        // LED+RRR,GGG,BBB
        sscanf(rxbuff, "LED+%3u,%3u,%3u", &red, &green, &blue);

        led_set(red, green, blue);

        sprintf(txbuff, "%s\n", COMMAND_SET_LED_RSP);
        uartSendString(txbuff);
#endif
    } else {
#ifdef DEBUG
        if (debug & DEBUG_COMMAND) {
            sprintf(txbuff, "UNKNOWN CMD: %s\n", rxbuff);
            uartSendString(txbuff);
        }
#endif
    }
}

void command_rx_handler(unsigned char c)
{
    static uint8_t i = 0;
    uint8_t len;

    rxbuff[i] = c;

#ifdef DEBUG
    if (rxbuff[i] == '\r') {
#else
    if (rxbuff[i] == '\n') {
#endif
        rxbuff[i] = '\0';

        len = i;
        i = 0;

        command_parse(len);
    } else {
        i++;

        if (i == RXBUFF_LEN) {
            i = 0;
        }
    }
}
