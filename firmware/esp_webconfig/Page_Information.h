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

#ifndef PAGE_INFOMATION_H
#define PAGE_INFOMATION_H

#include "ntp.h"
#include "common.h"

const char PAGE_Information[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <link rel="stylesheet" href="style.css" type="text/css" />
  <script src="microajax.js"></script>

  <div id="menu">
    &nbsp;<a href="config.html" style="width:25px; text-align:center;font-weight:bold" class="btn"><</a>&nbsp;&nbsp;<strong>Informace o síti</strong>
  </div
  <br><br>
  <table border="0" style="width:310px" class="center">
    <tr>
      <td align="right">Jméno:</td>
      <td><span id="x_hostname"></span></td>
    </tr>
    <tr>
      <td align="right">Stav:</td>
      <td><span id="x_connectionstate"></span></td>
    </tr>
    <tr>
      <td align="right">SSID:</td>
      <td><span id="x_ssid"></span></td>
    </tr>
    <tr>
      <td align="right">IP:</td>
      <td><span id="x_ip"></span></td>
    </tr>
    <tr>
      <td align="right">Maska:</td>
      <td><span id="x_netmask"></span></td>
    </tr>
    <tr>
      <td align="right">Brána:</td>
      <td><span id="x_gateway"></span></td>
    </tr>
    <tr>
      <td align="right">MAC:</td>
      <td><span id="x_mac"></span></td>
    </tr>
    <tr>
      <td colspan="2"><hr></span></td>
    </tr>
    <tr>
      <td align="right">NTP čas:</td>
      <td><span id="x_ntp"></span></td>
    </tr>
    <tr>
      <td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--green">Obnovit</a></td>
    </tr>
  </table>

  <script>
    function GetState()
    {
      setValues("/admin/infovalues");
    }

    window.onload = function ()
    {
      load("style.css","css", function()
      {
        load("microajax.js","js", function()
        {
          GetState();
        });
      });
    }
    function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
  </script>
)=====";

void send_information_values_html()
{
  String state = "N/A";
  String values = "";
  String hour = "";
  String minute = "";
  String second = "";
  String day = "";
  String month = "";
  DateTime localTime;

  if (WiFi.status() == 0)
    state = "Nečinný";
  else if (WiFi.status() == 1)
    state = "Žádne dostupné SSID";
  else if (WiFi.status() == 2)
    state = "Hledáni dokončeno";
  else if (WiFi.status() == 3)
    state = "Připojen";
  else if (WiFi.status() == 4)
    state = "Spojení selhalo";
  else if (WiFi.status() == 5)
    state = "Spojení ztraceno";
  else if (WiFi.status() == 6)
    state = "Odpojený";

  LocalTime(utcTime, &localTime);

  if (localTime.hour < 10) {
    hour = "0" + (String) localTime.hour;
  } else {
    hour = (String) localTime.hour;
  }

  if (localTime.minute < 10) {
    minute = "0" + (String) localTime.minute;
  } else {
    minute = (String) localTime.minute;
  }

  if (localTime.second < 10) {
    second = "0" + (String) localTime.second;
  } else {
    second = (String) localTime.second;
  }

  if (localTime.day < 10) {
    day = "0" + (String) localTime.day;
  } else {
    day = (String) localTime.day;
  }

  if (localTime.month < 10) {
    month = "0" + (String) localTime.month;
  } else {
    month = (String) localTime.month;
  }

  values += "x_connectionstate|" + state + "|div\n";
  values += "x_hostname|" + (String) config.hostname + "|div\n";
  values += "x_ssid|" + (String) WiFi.SSID() + "|div\n";
  values += "x_ip|" + (String) WiFi.localIP()[0] + "." +
            (String) WiFi.localIP()[1] + "." + (String) WiFi.localIP()[2] +
            "." + (String) WiFi.localIP()[3] + "|div\n";
  values += "x_gateway|" + (String) WiFi.gatewayIP()[0] + "." +
            (String) WiFi.gatewayIP()[1] + "." + (String) WiFi.gatewayIP()[2] +
            "." + (String) WiFi.gatewayIP()[3] + "|div\n";
  values += "x_netmask|" + (String) WiFi.subnetMask()[0] + "." +
            (String) WiFi.subnetMask()[1] + "." +
            (String) WiFi.subnetMask()[2] + "." +
            (String) WiFi.subnetMask()[3] + "|div\n";
  values += "x_mac|" + getMacAddress() + "|div\n";
  values += "x_ntp|" + hour + ":" + minute + ":" + second + " " +
            day + "." + month + "." + (String) localTime.year + "|div\n";
  server.send(200, "text/plain", values);
}

#endif
