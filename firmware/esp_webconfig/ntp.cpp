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
#include "log.h"
#include "ntp.h"

unsigned long utcTime;

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

int ntpUpdateTimeout = 0;
bool waitingNtpResponse = false;

static const uint8_t weekDays[] = { 7, 1, 2, 3, 4, 5, 6 };
static const uint8_t monthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#define LEAP_YEAR(Y) (((1970 + Y) > 0) && !((1970 + Y) % 4) && (((1970 + Y) % 100) || !((1970 + Y) % 400)))

bool IsSummerTime(int year, byte month, byte day, byte hour, long tz)
{
  if (month < 3 || month > 10)
    return false;
  if (month > 3 && month < 10)
    return true;
  if (((month == 3) && ((hour + 24 * day) >= (1 + (tz / 10.0) + 24 * (31 - (5 * year / 4 + 4) % 7)))) ||
      ((month == 10) && ((hour + 24 * day) < (1 + (tz / 10.0) + 24 * (31 - (5 * year / 4 + 1) % 7)))))
    return true;
  else
    return false;
}

void EpochToDateTime(unsigned long epoch, struct DateTime *dt)
{
  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t) epoch;
  dt->second = time % 60;
  time /= 60;    // now it is minutes
  dt->minute = time % 60;
  time /= 60;    // now it is hours
  dt->hour = time % 24;
  time /= 24;    // now it is days
  dt->wday = weekDays[(time + 4) % 7];

  year = 0;
  days = 0;

  while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }

  dt->year = year;

  days -= LEAP_YEAR(year) ? 366 : 365;
  time -= days;

  days = 0;
  month = 0;
  monthLength = 0;

  for (month = 0; month < 12; month++) {
    if (month == 1) {
      if (LEAP_YEAR(year)) {
        monthLength = 29;
      } else {
        monthLength = 28;
      }
    } else {
      monthLength = monthDays[month];
    }

    if (time >= monthLength) {
      time -= monthLength;
    } else {
      break;
    }
  }

  dt->month = month + 1;
  dt->day = time + 1;
  dt->year += 1970;
}

void LocalTime(unsigned long epoch, struct DateTime *dt)
{
  DateTime temp;

  EpochToDateTime(epoch + (config.timezone * 360) , &temp);

  if (config.daylight) {
    if (IsSummerTime(temp.year, temp.month, temp.day, temp.hour, config.timezone)) {
      EpochToDateTime(epoch + (config.timezone * 360) + 3600, dt);
    } else {
      *dt = temp;
    }
  } else {
    *dt = temp;
  }
}

void ntpUpdate(void)
{
  if (config.ntpUpdateTime > 0) {
#ifdef SERIAL_DEBUG
    Serial.print("DEBUG: Sending NTP request to ");
    Serial.println(config.ntpServerName.c_str());
#endif

#ifdef ENABLE_LOGGING
    AddLog(LOG_EVENT_NTP_REQUEST, 0);
#endif

    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;  // Stratum, or type of clock
    packetBuffer[2] = 6;  // Polling Interval
    packetBuffer[3] = 0xEC; // Peer Clock Precision
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    udp.beginPacket(config.ntpServerName.c_str(), 123);
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();

    waitingNtpResponse = true;
    ntpUpdateTimeout = 0;
  }
}

void ntpResponse(void)
{
  waitingNtpResponse = false;

  udp.read(packetBuffer, NTP_PACKET_SIZE);

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  const unsigned long seventyYears = 2208988800UL;
  unsigned long epoch = secsSince1900 - seventyYears;

#ifdef SERIAL_DEBUG
  Serial.print("DEBUG: Unix time = ");
  Serial.println(epoch);
#endif

#ifdef ENABLE_LOGGING
    AddLog(LOG_EVENT_NTP_RESPONSE, epoch);
#endif

  utcTime = epoch;
}
