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

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <IPAddress.h>

#define DEFAULT_SSID "Sunrise Clock"
#define DEFAULT_PASSWORD "12345678"

#define DEFAULT_HOSTNAME "sunriseclock"

extern bool configMode;

struct Alarm {
  bool enabled;
  byte hour;
  byte minute;
};

struct Configuration {
  String ssid;
  String password;

  bool dhcpEnabled;
  byte ipAddress[4];
  byte netmask[4];
  byte gateway[4];

  long timezone;
  bool daylight;

  String ntpServerName;
  long ntpUpdateTime;

  String hostname;

  Alarm alarm[7];
};

extern Configuration config;

void WriteConfig(void);
void WriteDefaultConfig(void);

bool ReadConfig(void);

void ConfigureConfigMode(void);
void ConfigureNetwork(void);

bool NetworkAvailable(void);

void SendPingCommand(void);
void SendGetStatusCommand(void);
void SendSetTimeCommand(void);
void SendSetAlarmCommand(int id);

#endif

