/*
 *  sunrise-alarmclock
 *  Copyright (C) 2017 Roman Moravcik
 *
 *  Based on:
 *  http://www.john-lassen.de/en/projects/esp-8266-arduino-ide-webconfig
 *
 *  Based on:
 *  https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/NTPClient/NTPClient.ino
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

#ifndef NTP_H
#define NTP_H

#include <Arduino.h>

struct DateTime {
  byte hour;
  byte minute;
  byte second;
  int year;
  byte month;
  byte day;
  byte wday;
};

extern unsigned long utcTime;

extern int ntpUpdateTimeout;
extern bool waitingNtpResponse;

#define IS_EPOCH_VALID(x)  ((x > 1483228800) ? true : false)

bool IsSummerTime(int year, byte month, byte day, byte hour, long tz);

void EpochToDateTime(unsigned long epoch, struct DateTime *dt);
void LocalTime(unsigned long epoch, struct DateTime *dt);

void ntpUpdate(void);
void ntpResponse(void);

#endif
