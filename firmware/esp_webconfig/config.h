#ifndef CONFIG_H
#define CONFIG_H

#include "IPAddress.h"

#define DEFAULT_SSID "Sunrise Clock"
#define DEFAULT_PASSWORD "12345678"

extern boolean configMode;
extern int connectionTimeout;

struct Configuration {
  String ssid;
  String password;

  boolean dhcpEnabled;
  byte ipAddress[4];
  byte netmask[4];
  byte gateway[4];

  long timezone;
  boolean daylight;

  String ntpServerName;
  long ntpUpdateTime;
};

extern Configuration config;

void WriteConfig();
void WriteDefaultConfig();

boolean ReadConfig();

void ConfigureConfigMode();
void ConfigureNetwork();

#endif

