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
#include <util/delay.h>

#include "ds_rtc_lib/library-gcc/twi.h"
#include "ds_rtc_lib/library-gcc/rtc.h"

#include "ds_rtc_lib/library-gcc/test/uart.h"

#define LED_BIT PB5
#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_HIGH LED_PORT |= _BV(LED_BIT)
#define LED_HIGH LED_PORT |= _BV(LED_BIT)
#define LED_LOW  LED_PORT &= ~(_BV(LED_BIT))

//void read_rtc(void)
//{
//	uint8_t hour, min, sec;
//
//	rtc_get_time_s(&hour, &min, &sec);
//}

void main(void) __attribute__ ((noreturn));

void main(void)
{
	struct tm* t = NULL;
	struct tm time;
	char buf[32];
	uint8_t hour, min, sec;

	uartInit();
	uartSetBaudRate(115200);
	uartSendString("DS RTC Library Test\r\n");

	LED_DDR  |= _BV(LED_BIT); // indicator led

	for (int i = 0; i < 5; i++) {
		LED_HIGH;
		_delay_ms(100);
		LED_LOW;
		_delay_ms(100);
	}

	twi_init_master();
	rtc_init();

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

	rtc_set_alarm_s(12, 1, 0);
	
	rtc_get_alarm_s(&hour, &min, &sec);

	sprintf(buf, "Alarm is set -%02d:%02d:%02d-\r\n", hour, min, sec);
	uartSendString(buf);
	uartSendString("---\r\n");
	uartSendString("---\r\n");
	uartSendString("---\r\n");

	while (1) {
		t = rtc_get_time();

		sprintf(buf, "%02d:%02d:%02d\r\n", t->hour, t->min, t->sec);
		uartSendString(buf);
		uartSendString("---\r\n");

		if (rtc_check_alarm())
			uartSendString("ALARM!\r\n");

	   	_delay_ms(500);
	}
}
