/*
  ESP_WebConfig 

  Copyright (c) 2015 John Lassen. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Latest version: 1.1.3  - 2015-07-20
  Changed the loading of the Javascript and CCS Files, so that they will successively loaded and that only one request goes to the ESP.

  -----------------------------------------------------------------------------------------------
  History

  Version: 1.1.2  - 2015-07-17
  Added URLDECODE for some input-fields (SSID, PASSWORD...)

  Version  1.1.1 - 2015-07-12
  First initial version to the public

  */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "helpers.h"
#include "global.h"
/*
Include the HTML, STYLE and Script "Pages"
*/
#include "Page_Root.h"
#include "Page_Configuration.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPSettings.h"
#include "Page_Information.h"
#include "PAGE_NetworkConfiguration.h"

#define DEFAULT_SSID "Sunrise Clock"
#define DEFAULT_PASSWORD "12345678"

#define AdminTimeOut 180	// Defines the Time in Seconds, when the Admin-Mode will be diabled

void setup(void)
{
	EEPROM.begin(512);
	Serial.begin(115200);
	delay(500);
	Serial.println("Starting ES8266");
	if (!ReadConfig()) {
		// DEFAULT CONFIG
		config.ssid = DEFAULT_SSID;
		config.password = DEFAULT_PASSWORD;
		config.dhcp = true;
		config.ip[0] = 192;
		config.ip[1] = 168;
		config.ip[2] = 1;
		config.ip[3] = 100;
		config.netmask[0] = 255;
		config.netmask[1] = 255;
		config.netmask[2] = 255;
		config.netmask[3] = 0;
		config.gateway[0] = 192;
		config.gateway[1] = 168;
		config.gateway[2] = 1;
		config.gateway[3] = 1;
		config.ntpServerName = "0.de.pool.ntp.org";
		config.ntpUpdateTime = 0;
		config.timezone = -10;
		config.daylight = true;
		WriteConfig();
		Serial.println("General config applied");
	}

	if (AdminEnabled) {
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP(DEFAULT_SSID, DEFAULT_PASSWORD);
	} else {
		WiFi.mode(WIFI_STA);
	}

	ConfigureWifi();

	server.on("/favicon.ico",[]() {
		  Serial.println("favicon.ico");
		  server.send(200, "text/html", "");}
	);

	server.on("/", send_root_html);

	server.on("/config.html", []() {
		  Serial.println("config.html");
		  server.send (200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_ConfigurationPage));}
	);

	server.on("/network.html", send_network_configuration_html);

	server.on("/info.html",[]() {
		  Serial.println("info.html");
		  server.send(200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_Information));}
	);
	server.on("/ntp.html", send_NTP_configuration_html);

	server.on("/style.css",[]() {
		  Serial.println("style.css");
		  server.send(200, "text/plain", reinterpret_cast<const __FlashStringHelper *>(PAGE_Style_css));}
	);

	server.on("/microajax.js",[]() {
		  Serial.println("microajax.js");
		  server.send(200, "text/plain", reinterpret_cast<const __FlashStringHelper *>(PAGE_microajax_js));}
	);

	server.on("/admin/rootvalues", send_root_values_html);

	server.on("/admin/networkvalues", send_network_configuration_values_html);

	server.on("/admin/connectionstate", send_connection_state_values_html);

	server.on("/admin/infovalues", send_information_values_html);

	server.on("/admin/ntpvalues", send_NTP_configuration_values_html);

	server.onNotFound([]() {
			  Serial.println("Page Not Found");
			  server.send(400, "text/html", "Page not Found");}
	);

	server.begin();
	Serial.println("HTTP server started");
	tkSecond.attach(1, Second_Tick);
	UDPNTPClient.begin(2390);	// Port for NTP receive
}

void loop(void)
{
	if (AdminEnabled) {
		if (AdminTimeOutCounter > AdminTimeOut) {
			AdminEnabled = false;
			Serial.println("Admin Mode disabled!");
			WiFi.mode(WIFI_STA);
		}
	}
	if (config.ntpUpdateTime > 0) {
		if (cNTP_Update > 5 && firstStart) {
			NTPRefresh();
			cNTP_Update = 0;
			firstStart = false;
		} else if (cNTP_Update > (config.ntpUpdateTime * 60)) {

			NTPRefresh();
			cNTP_Update = 0;
		}
	}

	if (DateTime.minute != Minute_Old) {
		Minute_Old = DateTime.minute;
	}
	server.handleClient();

	/*
	 *    Your Code here
	 */

	if (Refresh) {
		Refresh = false;
		///Serial.println("Refreshing...");
		//Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
	}

}
