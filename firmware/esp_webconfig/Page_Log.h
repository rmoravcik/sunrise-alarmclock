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

#ifndef PAGE_LOG_H
#define PAGE_LOG_H

#ifdef ENABLE_LOGGING

#include "log.h"
#include "common.h"

const char PAGE_Log[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <link rel="stylesheet" href="style.css" type="text/css" />
  <script src="microajax.js"></script>

  <div id="menu">
    &nbsp;<a href="config.html" style="width:25px; text-align:center;font-weight:bold" class="btn"><</a>&nbsp;&nbsp;<strong>Log</strong>
  </div
  <br><br>

  <table border="0" style="width:310px" class="center">
  <tr>
    <td style="padding:0px"><div id="log"><span style="padding:12px">Loading...</span></div></td>
  </tr>
  <tr>
    <td align="center"><a href="javascript:GetState()" style="width:150px" class="btn btn--m btn--green">Refresh</a></td>
  </tr>
  </table>

  <script>
    function GetState()
    {
      setValues("/admin/logvalues");
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

void send_log_values_html()
{
  String Entries = "";

  Entries += "<table border='0' style='width:310px' class='center'>";
  Entries += "<tr bgcolor='#DDDDDD' ><td><strong>Timestamp</strong></td><td><strong>Event</strong></td><td><strong>Param</strong></td><tr>";

  byte index = 0;
  if (LogSize == LOG_SIZE) {
    index = LogIndex++;
    if (index >= LOG_SIZE)
      index = 0;
  }

  for (byte i = 0; i < LogSize; ++i) {
    String event = "";

    switch (Log[index].event) {
      case LOG_EVENT_WIFI_DISCONNECTED:
        event = "WIFI_DISCONNECTED";
        break;
      case LOG_EVENT_WIFI_CONNECTED:
        event = "WIFI_CONNECTED";
        break;
      case LOG_EVENT_NTP_REQUEST:
        event = "NTP_REQUEST";
        break;
      case LOG_EVENT_NTP_RESPONSE:
        event = "NTP_RESPONSE";
        break;
      case LOG_EVENT_NTP_RESPONSE_TIMEOUT:
        event = "NTP_RESPONSE_TIMEOUT";
        break;
      case LOG_EVENT_PING:
        event = "PING";
        break;
      case LOG_EVENT_GET_STATUS:
        event = "GET_STATUS";
        break;
      case LOG_EVENT_SET_TIME:
        event = "SET_TIME";
        break;
      case LOG_EVENT_SET_ALARM:
        event = "SET_ALARM";
        break;
      default:
        event = String(Log[index].event);
    }

    Entries += "<tr><td>" + String(Log[index].timestamp) + "</td><td>" +
               event + "</td><td>" +
               String(Log[index].parameter) + "</td></tr>";
    index++;
    if (index >= LOG_SIZE)
      index = 0;
  }

  Entries += "</table>";

  String values = "";

  values += "log|" + Entries + "|div\n";

  server.send(200, "text/plain", values);
}
#endif

#endif
