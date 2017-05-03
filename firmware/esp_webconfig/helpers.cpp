/*
 *  sunrise-alarmclock
 *  Copyright (C) 2017 Roman Moravcik
 *
 *  Based on:
 *  http://www.john-lassen.de/en/projects/esp-8266-arduino-ide-webconfig
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

#include <Arduino.h>

#include "common.h"
#include "helpers.h"

bool checkRange(String Value)
{
  if (Value.toInt() < 0 || Value.toInt() > 255) {
    return false;
  } else {
    return true;
  }
}

bool checkTime(String Value)
{
  int hour = Value.substring(0,2).toInt();
  int minute = Value.substring(3,5).toInt();

  if (hour < 0 || hour > 23) {
    return false;
  }

  if (minute < 0 || minute > 59) {
    return false;
  }

  return true;
}

String getMacAddress(void)
{
  uint8_t mac[6];
  char macStr[18] = { 0 };
  WiFi.macAddress(mac);
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2],
    mac[3], mac[4], mac[5]);
  return String(macStr);
}

// convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
static unsigned char h2int(char c)
{
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

String urlDecode(String input)  // (based on https://code.google.com/p/avr-netino/)
{
  char c;
  String ret = "";

  for (byte t = 0; t < input.length(); t++) {
    c = input[t];
    if (c == '+')
      c = ' ';
    if (c == '%') {

      t++;
      c = input[t];
      t++;
      c = (h2int(c) << 4) | h2int(input[t]);
    }

    ret.concat(c);
  }
  return ret;
}

String IPAddressToString(IPAddress address)
{
  String res = "";

  for (int i = 0; i < 3; i++) {
    res += String((address >> (8 * i)) & 0xFF) + ".";
  }

  res += String(((address >> 8 * 3)) & 0xFF);
  return res;
}
