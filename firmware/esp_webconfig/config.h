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
  byte min;
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

#endif

