#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "helpers.h"

#include "Page_Root.h"
#include "Page_Configuration.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPSettings.h"
#include "Page_Information.h"
#include "Page_NetworkConfiguration.h"

#include "config.h"
#include "common.h"

ESP8266WebServer server(80);
DNSServer dns;
Ticker tkSecond;

bool mDNSResponse = false;

void Tick()
{
  if (!configMode) {
    if (WiFi.status() != WL_CONNECTED) {
      connectionTimeout++;
      mDNSResponse = false;
    } else {
      if (mDNSResponse == false) {
        Serial.println(WiFi.localIP());
        MDNS.begin("sunrisealarm");
        mDNSResponse = true;
      }

      connectionTimeout = 0;
    }

    if (connectionTimeout > 30) {
      ConfigureConfigMode();
    }
  }

#if 0
  strDateTime tempDateTime;
  AdminTimeOutCounter++;
  cNTP_Update++;
  UnixTimestamp++;

  ConvertUnixTimeStamp(UnixTimestamp + (config.timezone * 360), &tempDateTime);

  if (config.daylight)
    if (summertime(tempDateTime.year, tempDateTime.month, tempDateTime.day, tempDateTime.hour, 0)) {
      ConvertUnixTimeStamp(UnixTimestamp + (config.timezone * 360) + 3600, &DateTime);
    } else {
      DateTime = tempDateTime;
  } else {
    DateTime = tempDateTime;
  }
#endif
}

void setup(void)
{
  EEPROM.begin(512);
  Serial.begin(115200);
  delay(500);

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

  server.on("/generate_204", []() {
      server.send (200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_ConfigurationPage));}
  );

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
        server.send(400, "text/html", "Page not Found");}
  );

  server.begin();

  tkSecond.attach(1, Tick);
//  UDPNTPClient.begin(2390);
}

void loop(void)
{
  if (configMode) {
    dns.processNextRequest();
  }

  server.handleClient();
}
