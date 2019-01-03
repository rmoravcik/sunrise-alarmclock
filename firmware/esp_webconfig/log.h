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

#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

#include "common.h"

#ifdef ENABLE_LOGGING

struct LogEntry {
  unsigned long timestamp;
  byte event;
  unsigned long parameter;
};

enum LogEvent {
  LOG_EVENT_WIFI_DISCONNECTED = 0,
  LOG_EVENT_WIFI_CONNECTED,
  LOG_EVENT_NTP_REQUEST,
  LOG_EVENT_NTP_RESPONSE,
  LOG_EVENT_NTP_RESPONSE_TIMEOUT,
  LOG_EVENT_PING,
  LOG_EVENT_GET_STATUS,
  LOG_EVENT_SET_TIME,
  LOG_EVENT_SET_ALARM,
};

#define LOG_SIZE 100

extern struct LogEntry Log[LOG_SIZE];

extern byte LogSize;
extern byte LogIndex;

void AddLog(byte event, unsigned long parameter);

#endif

#endif
