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

#include <ESP8266WiFi.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

#include "Page_Root.h"
#include "Page_Configuration.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPSettings.h"
#include "Page_Information.h"
#include "Page_NetworkConfiguration.h"

#include "config.h"
#include "helpers.h"
#include "common.h"

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer updater;
DNSServer dns;
WiFiUDP udp;
Ticker tkSecond;

int connectionTimeout = 0;
int ntpUpdateRetryCount = 0;
bool mdnsResponseSent = false;
int summerTime = -1;

void Tick()
{
  connectionTimeout++;
  utcTime++;

  if (waitingNtpResponse) {
    if (ntpUpdateTimeout > 10) {
#ifdef SERIAL_DEBUG
          Serial.println("DEBUG: Timeout waiting for NTP response");
#endif
      waitingNtpResponse = false;
      ntpUpdateRetryCount++;

      if (ntpUpdateRetryCount < 3) {
        ntpUpdateTimeout = config.ntpUpdateTime * 60;
      }
    }
  }

  if (config.ntpUpdateTime > 0) {
    ntpUpdateTimeout++;
  }

  if (config.daylight) {
    if (IS_EPOCH_VALID(utcTime)) {
      DateTime tmp;
      int summer = 0;
      long localTime = utcTime + (config.timezone * 360);

      EpochToDateTime(localTime, &tmp);
      if (IsSummerTime(tmp.year, tmp.month, tmp.day, tmp.hour, config.timezone)) {
          summer = 1;
      }

      if (summerTime == -1) {
        if (summer) {
          summerTime = 1;
        } else {
          summerTime = 0;
        }
      } else {
        if (summerTime != summer) {
          SendSetTimeCommand();
          summerTime = !summerTime;
        }        
      }
    }
  }
}

void setup(void)
{
  EEPROM.begin(512);
  Serial.begin(38400);
  delay(500);

  connectionTimeout = 0;

  if (!ReadConfig()) {
    WriteDefaultConfig();
    ConfigureConfigMode();
  } else {
    ConfigureNetwork();
  }

  server.on("/favicon.ico",[]() {
    server.send(200, "text/html", "");}
  );

  server.on("/", send_root_html);

  server.on("/config.html", []() {
    server.send (200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_ConfigurationPage));}
  );

  server.on("/network.html", send_network_configuration_html);

  server.on("/info.html",[]() {
    server.send(200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_Information));}
  );
  server.on("/ntp.html", send_NTP_configuration_html);

  server.on("/style.css",[]() {
    server.send(200, "text/plain", reinterpret_cast<const __FlashStringHelper *>(PAGE_Style_css));}
  );

  server.on("/microajax.js",[]() {
    server.send(200, "text/plain", reinterpret_cast<const __FlashStringHelper *>(PAGE_microajax_js));}
  );

  server.on("/admin/rootvalues", send_root_values_html);

  server.on("/admin/networkvalues", send_network_configuration_values_html);

  server.on("/admin/connectionstate", send_connection_state_values_html);

  server.on("/admin/infovalues", send_information_values_html);

  server.on("/admin/ntpvalues", send_NTP_configuration_values_html);

  server.onNotFound([]() {
    Serial.println(server.hostHeader());
    if (configMode) {
      server.sendHeader("Location", "http://" + IPAddressToString(server.client().localIP()) + "/config.html", true);
      server.send(302, "text/plain", "");
      server.client().stop();
    } else {
      server.send(400, "text/html", "Page not Found");}    
    }
  );

  updater.setup(&server);
  server.begin();

  tkSecond.attach(1, Tick);

  udp.begin(2390);

  SendPingCommand();
  SendGetStatusCommand();
}

void loop(void)
{
  if (waitingNtpResponse) {
    if (udp.parsePacket()) {
      ntpUpdateRetryCount = 0;
      ntpResponse();
      SendSetTimeCommand();
    }
  }

  if (configMode) {
    if (connectionTimeout >= 30) {
#ifdef SERIAL_DEBUG
      Serial.println("DEBUG: Scanning available networks");
#endif
      if (NetworkAvailable()) {
        ConfigureNetwork();
      }

      connectionTimeout = 0;
    }

    dns.processNextRequest();
  } else {
    if (connectionTimeout >= 30) {
#ifdef SERIAL_DEBUG
      Serial.println("DEBUG: Connection timeout");
#endif
      ConfigureConfigMode();

      connectionTimeout = 0;
      mdnsResponseSent = false;
    }

    if (WiFi.status() == WL_CONNECTED) {
      if (!mdnsResponseSent) {
#ifdef SERIAL_DEBUG
          Serial.println("DEBUG: Sending mDNS response");
#endif
          MDNS.begin(config.hostname.c_str());
          MDNS.addService("http", "tcp", 80);
          mdnsResponseSent = true;

          ntpUpdate();
      }

      if (ntpUpdateTimeout >= config.ntpUpdateTime * 60) {
        ntpUpdate();
      }

      connectionTimeout = 0;
    }
  }

  server.handleClient();
}
