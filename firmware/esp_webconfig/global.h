#ifndef GLOBAL_H
#define GLOBAL_H

ESP8266WebServer server(80);	// The Webserver
boolean firstStart = true;	// On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;	// Counter for Disabling the AdminMode
strDateTime DateTime;		// Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;		// NTP Client
unsigned long UnixTimestamp = 0;	// GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false;	// For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;		// Counter for Updating the time via NTP
Ticker tkSecond;		// Second - Timer for Updating Datetime Structure
boolean AdminEnabled = true;	// Enable Admin Mode for a given Time
byte Minute_Old = 100;		// Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)

struct strConfig {
	String ssid;
	String password;
	byte ip[4];
	byte netmask[4];
	byte gateway[4];
	boolean dhcp;
	String ntpServerName;
	long ntpUpdateTime;
	long timezone;
	boolean daylight;
} config;

/*
**
** CONFIGURATION HANDLING
**
*/
void ConfigureWifi()
{
	Serial.println("Configuring Wifi");

	WiFi.begin(config.ssid.c_str(), config.password.c_str());

	if (!config.dhcp) {
		WiFi.config(IPAddress(config.ip[0], config.ip[1], config.ip[2], config.ip[3]),
		            IPAddress(config.gateway[0], config.gateway[1], config.gateway[2], config.gateway[3]),
		            IPAddress(config.netmask[0], config.netmask[1], config.netmask[2], config.netmask[3]));
	}
}

void WriteConfig()
{
	Serial.println("Writing Config");
	EEPROM.write(0, 'C');
	EEPROM.write(1, 'F');
	EEPROM.write(2, 'G');

	EEPROM.write(16, config.dhcp);
	EEPROM.write(17, config.daylight);

	EEPROMWritelong(18, config.ntpUpdateTime);	// 4 Byte

	EEPROMWritelong(22, config.timezone);	// 4 Byte

	EEPROM.write(32, config.ip[0]);
	EEPROM.write(33, config.ip[1]);
	EEPROM.write(34, config.ip[2]);
	EEPROM.write(35, config.ip[3]);

	EEPROM.write(36, config.netmask[0]);
	EEPROM.write(37, config.netmask[1]);
	EEPROM.write(38, config.netmask[2]);
	EEPROM.write(39, config.netmask[3]);

	EEPROM.write(40, config.gateway[0]);
	EEPROM.write(41, config.gateway[1]);
	EEPROM.write(42, config.gateway[2]);
	EEPROM.write(43, config.gateway[3]);

	WriteStringToEEPROM(64, config.ssid);
	WriteStringToEEPROM(96, config.password);
	WriteStringToEEPROM(128, config.ntpServerName);

	EEPROM.commit();
}

boolean ReadConfig()
{

	Serial.println("Reading Configuration");
	if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F' && EEPROM.read(2) == 'G') {
		Serial.println("Configurarion Found!");
		config.dhcp = EEPROM.read(16);

		config.daylight = EEPROM.read(17);

		config.ntpUpdateTime = EEPROMReadlong(18);	// 4 Byte

		config.timezone = EEPROMReadlong(22);	// 4 Byte

		config.ip[0] = EEPROM.read(32);
		config.ip[1] = EEPROM.read(33);
		config.ip[2] = EEPROM.read(34);
		config.ip[3] = EEPROM.read(35);
		config.netmask[0] = EEPROM.read(36);
		config.netmask[1] = EEPROM.read(37);
		config.netmask[2] = EEPROM.read(38);
		config.netmask[3] = EEPROM.read(39);
		config.gateway[0] = EEPROM.read(40);
		config.gateway[1] = EEPROM.read(41);
		config.gateway[2] = EEPROM.read(42);
		config.gateway[3] = EEPROM.read(43);
		config.ssid = ReadStringFromEEPROM(64);
		config.password = ReadStringFromEEPROM(96);
		config.ntpServerName = ReadStringFromEEPROM(128);
		return true;
	} else {
		Serial.println("Configurarion NOT FOUND!!!!");
		return false;
	}
}

/*
**
**  NTP 
**
*/
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

void NTPRefresh()
{
	if (WiFi.status() == WL_CONNECTED) {
		IPAddress timeServerIP;

		WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP);
		//sendNTPpacket(timeServerIP); // send an NTP packet to a time server

		Serial.println("sending NTP packet...");

		memset(packetBuffer, 0, NTP_PACKET_SIZE);
		packetBuffer[0] = 0b11100011;	// LI, Version, Mode
		packetBuffer[1] = 0;	// Stratum, or type of clock
		packetBuffer[2] = 6;	// Polling Interval
		packetBuffer[3] = 0xEC;	// Peer Clock Precision
		packetBuffer[12] = 49;
		packetBuffer[13] = 0x4E;
		packetBuffer[14] = 49;
		packetBuffer[15] = 52;

		UDPNTPClient.beginPacket(timeServerIP, 123);
		UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
		UDPNTPClient.endPacket();

		delay(1000);

		int cb = UDPNTPClient.parsePacket();
		if (!cb) {
			Serial.println("NTP no packet yet");
		} else {
			Serial.print("NTP packet received, length=");
			Serial.println(cb);
			UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE);	// read the packet into the buffer
			unsigned long highWord =
			    word(packetBuffer[40], packetBuffer[41]);
			unsigned long lowWord =
			    word(packetBuffer[42], packetBuffer[43]);
			unsigned long secsSince1900 = highWord << 16 | lowWord;
			const unsigned long seventyYears = 2208988800UL;
			unsigned long epoch = secsSince1900 - seventyYears;
			UnixTimestamp = epoch;
		}
	}
}

void Second_Tick()
{
	strDateTime tempDateTime;
	AdminTimeOutCounter++;
	cNTP_Update++;
	UnixTimestamp++;

	ConvertUnixTimeStamp(UnixTimestamp + (config.timezone * 360), &tempDateTime);

	if (config.daylight)	// Sommerzeit beachten
		if (summertime(tempDateTime.year, tempDateTime.month, tempDateTime.day, tempDateTime.hour, 0)) {
			ConvertUnixTimeStamp(UnixTimestamp + (config.timezone * 360) + 3600, &DateTime);
		} else {
			DateTime = tempDateTime;
	} else {
		DateTime = tempDateTime;
	}

	Refresh = true;
}

#endif

