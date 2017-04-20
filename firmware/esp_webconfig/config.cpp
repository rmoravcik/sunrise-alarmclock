#include <EEPROM.h>

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
    EEPROM.write(155 + 3 * i, config.alarm[i].min);
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
    config.alarm[i].min = 0;
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
      config.alarm[i].min = EEPROM.read(155 + 3 * i);

      if ((config.alarm[i].hour > 23) || (config.alarm[i].min > 59)) {
        config.alarm[i].enabled = false;
        config.alarm[i].hour = 0;
        config.alarm[i].min = 0;
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
  Serial.println("Switching to config mode");
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
  Serial.print("Connecting to network ");
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

