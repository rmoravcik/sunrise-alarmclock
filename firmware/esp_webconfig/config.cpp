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

#include <EEPROM.h>

#include "ntp.h"
#include "common.h"
#include "config.h"

IPAddress adminIpAddress(192, 168, 1, 100);
IPAddress adiminNetmask(255, 255, 255, 0);

Configuration config;
bool configMode = false;

static void WriteStringToEEPROM(int beginaddress, String string)
{
  char charBuf[string.length() + 1];
  string.toCharArray(charBuf, string.length() + 1);
  for (unsigned int t = 0; t < sizeof(charBuf); t++) {
    EEPROM.write(beginaddress + t, charBuf[t]);
  }
}

static String ReadStringFromEEPROM(int beginaddress)
{
  byte counter = 0;
  char rChar;
  String retString = "";
  while (1) {
    rChar = EEPROM.read(beginaddress + counter);
    if (rChar == 0)
      break;
    if (counter > 31)
      break;
    counter++;
    retString.concat(rChar);

  }
  return retString;
}

static void EEPROMWritelong(int address, long value)
{
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

static long EEPROMReadlong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) +
      ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void WriteConfig(void)
{
  EEPROM.write(0, 'C');
  EEPROM.write(1, 'F');
  EEPROM.write(2, 'G');

  WriteStringToEEPROM(3, config.ssid);
  WriteStringToEEPROM(35, config.password);

  EEPROM.write(67, config.dhcpEnabled);
  EEPROM.write(68, config.ipAddress[0]);
  EEPROM.write(69, config.ipAddress[1]);
  EEPROM.write(70, config.ipAddress[2]);
  EEPROM.write(71, config.ipAddress[3]);
  EEPROM.write(72, config.netmask[0]);
  EEPROM.write(73, config.netmask[1]);
  EEPROM.write(74, config.netmask[2]);
  EEPROM.write(75, config.netmask[3]);
  EEPROM.write(76, config.gateway[0]);
  EEPROM.write(77, config.gateway[1]);
  EEPROM.write(78, config.gateway[2]);
  EEPROM.write(79, config.gateway[3]);

  EEPROMWritelong(80, config.timezone);
  EEPROM.write(84, config.daylight);

  EEPROMWritelong(85, config.ntpUpdateTime);
  WriteStringToEEPROM(89, config.ntpServerName);

  WriteStringToEEPROM(121, config.hostname);

  for (int i = 0; i < 7; i++) {
    EEPROM.write(153 + 3 * i, config.alarm[i].enabled);
    EEPROM.write(154 + 3 * i, config.alarm[i].hour);
    EEPROM.write(155 + 3 * i, config.alarm[i].minute);
  }

  EEPROM.commit();
}

void WriteDefaultConfig(void)
{
  config.ssid = DEFAULT_SSID;
  config.password = DEFAULT_PASSWORD;

  config.dhcpEnabled = true;
  config.ipAddress[0] = adminIpAddress[0];
  config.ipAddress[1] = adminIpAddress[1];
  config.ipAddress[2] = adminIpAddress[2];
  config.ipAddress[3] = adminIpAddress[3];
  config.netmask[0] = adiminNetmask[0];
  config.netmask[1] = adiminNetmask[1];
  config.netmask[2] = adiminNetmask[2];
  config.netmask[3] = adiminNetmask[3];
  config.gateway[0] = 192;
  config.gateway[1] = 168;
  config.gateway[2] = 1;
  config.gateway[3] = 1;

  config.timezone = 1;
  config.daylight = true;

  config.ntpServerName = "pool.ntp.org";
  config.ntpUpdateTime = 0;

  config.hostname = DEFAULT_HOSTNAME;

  for (int i = 0; i < 7; i++) {
    config.alarm[i].enabled = false;
    config.alarm[i].hour = 0;
    config.alarm[i].minute = 0;
  }

  WriteConfig();
}

bool ReadConfig(void)
{
  if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F' && EEPROM.read(2) == 'G') {
    config.ssid = ReadStringFromEEPROM(3);
    config.password = ReadStringFromEEPROM(35);

    config.dhcpEnabled = EEPROM.read(67);
    config.ipAddress[0] = EEPROM.read(68);
    config.ipAddress[1] = EEPROM.read(69);
    config.ipAddress[2] = EEPROM.read(70);
    config.ipAddress[3] = EEPROM.read(71);
    config.netmask[0] = EEPROM.read(72);
    config.netmask[1] = EEPROM.read(73);
    config.netmask[2] = EEPROM.read(74);
    config.netmask[3] = EEPROM.read(75);
    config.gateway[0] = EEPROM.read(76);
    config.gateway[1] = EEPROM.read(77);
    config.gateway[2] = EEPROM.read(78);
    config.gateway[3] = EEPROM.read(79);

    config.timezone = EEPROMReadlong(80);
    config.daylight = EEPROM.read(84);

    config.ntpUpdateTime = EEPROMReadlong(85);
    config.ntpServerName = ReadStringFromEEPROM(89);

    config.hostname = ReadStringFromEEPROM(121);

    for (int i = 0; i < 7; i++) {
      config.alarm[i].enabled = EEPROM.read(153 + 3 * i);
      config.alarm[i].hour = EEPROM.read(154 + 3 * i);
      config.alarm[i].minute = EEPROM.read(155 + 3 * i);

      if ((config.alarm[i].hour > 23) || (config.alarm[i].minute > 59)) {
        config.alarm[i].enabled = false;
        config.alarm[i].hour = 0;
        config.alarm[i].minute = 0;
      }
    }

    return true;
  } else {
    return false;
  }
}

void ConfigureConfigMode(void)
{
#ifdef SERIAL_DEBUG
  Serial.println("DEBUG: Switching to config mode");
#endif

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(adminIpAddress, adminIpAddress, adiminNetmask);
  WiFi.softAP(DEFAULT_SSID, DEFAULT_PASSWORD);

  dns.setErrorReplyCode(DNSReplyCode::NoError);
  dns.start(53, "*", adminIpAddress);
  configMode = true;
}

void ConfigureNetwork(void)
{
#ifdef SERIAL_DEBUG
  Serial.print("DEBUG: Connecting to network ");
  Serial.println(config.ssid.c_str());
#endif

  dns.stop();
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid.c_str(), config.password.c_str());

  if (!config.dhcpEnabled) {
    WiFi.config(IPAddress(config.ipAddress[0],
                          config.ipAddress[1],
                          config.ipAddress[2],
                          config.ipAddress[3]),
                IPAddress(config.gateway[0],
                          config.gateway[1],
                          config.gateway[2],
                          config.gateway[3]),
                IPAddress(config.netmask[0],
                          config.netmask[1],
                          config.netmask[2],
                          config.netmask[3]));
  }
  configMode = false;
}

bool NetworkAvailable(void)
{
  int n = WiFi.scanNetworks();

  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == config.ssid) {
      return true;
    }
  }
  return false;
}

static void flushSerial(void)
{
  while (Serial.available() > 0) {
    int tmp = Serial.read();
  }
}

void SendPingCommand(void)
{
  flushSerial();
  Serial.print("PING?\n");
}

void SendGetStatusCommand(void)
{
  String response = "";

  flushSerial();
  Serial.print("STAT?\n");

  Serial.setTimeout(100);
  response = Serial.readString();

  if (response.startsWith("STAT+")) {
    response.remove(0, 5);

    for (int i = 0; i < 7; i++) {
      int hour = response.substring(0, 2).toInt();
      int minute = response.substring(3, 5).toInt();
      int conf_hour = config.alarm[i].hour;
      int conf_minute = config.alarm[i].minute;

      if (!config.alarm[i].enabled) {
        conf_hour = conf_minute = 99;
      }

      if ((conf_hour != hour) || (conf_minute != minute)) {
        SendSetAlarmCommand(i);
      }

      if (i < 6) {
        response.remove(0, 6);
      }
    }
  }
}

void SendSetTimeCommand(void)
{
  // check if we have a valid timestamp
  if (IS_EPOCH_VALID(utcTime)) {
    String values = "";
    DateTime localTime;

    LocalTime(utcTime, &localTime);

    // DATE+HH:mm:ss;F;dd.MM.yyyy
    values = "DATE+";

    if (localTime.hour < 10) {
      values += "0" + (String) localTime.hour;
    } else {
      values += (String) localTime.hour;
    }

    values += ":";

    if (localTime.minute < 10) {
      values += "0" + (String) localTime.minute;
    } else {
      values += (String) localTime.minute;
    }

    values += ":";

    if (localTime.second < 10) {
      values += "0" + (String) localTime.second;
    } else {
      values += (String) localTime.second;
    }

    values += ";" + (String) localTime.wday + ";";

    if (localTime.day < 10) {
      values += "0" + (String) localTime.day;
    } else {
      values += (String) localTime.day;
    }

    values += ".";

    if (localTime.month < 10) {
      values += "0" + (String) localTime.month;
    } else {
      values += (String) localTime.month;
    }

    values += "." + (String) localTime.year;

    Serial.print(values);
    Serial.print("\n");
  }
}

void SendSetAlarmCommand(int id)
{
  String values = "";

  // ALARM+F;HH:mm
  values = "ALARM+" + (String) id + ";";

  if (config.alarm[id].enabled) {
    if (config.alarm[id].hour < 10) {
      values += "0" + (String) config.alarm[id].hour;
    } else {
      values += (String) config.alarm[id].hour;
    }

    values += ":";

    if (config.alarm[id].minute < 10) {
      values += "0" + (String) config.alarm[id].minute;
    } else {
      values += (String) config.alarm[id].minute;
    }
  } else {
    values += "99:99";
  }

  flushSerial();
  Serial.print(values);
  Serial.print("\n");
}
