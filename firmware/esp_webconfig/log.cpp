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

#include "config.h"
#include "common.h"
#include "ntp.h"
#include "log.h"

#ifdef ENABLE_LOGGING

struct LogEntry Log[LOG_SIZE];

byte LogSize = 0;
byte LogIndex = 0;

void AddLog(byte event, unsigned long parameter)
{
  Log[LogIndex].timestamp = utcTime;
  Log[LogIndex].event = event;
  Log[LogIndex].parameter = parameter;

  if (LogSize < LOG_SIZE)
    LogSize++;

  if (LogIndex < (LOG_SIZE - 1))
    LogIndex++;
  else
    LogIndex = 0;
}

#endif
