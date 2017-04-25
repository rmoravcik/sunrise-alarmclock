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

#ifndef PAGE_NTPSETTINGS
#define PAGE_NTPSETTINGS

#include "config.h"
#include "helpers.h"
#include "ntp.h"

#include "common.h"

const char PAGE_NTPConfiguration[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <link rel="stylesheet" href="style.css" type="text/css" />
  <script src="microajax.js"></script>

  <div id="menu">
    &nbsp;<a href="config.html" style="width:25px; text-align:center;font-weight:bold" class="btn"><</a>&nbsp;&nbsp;<strong>Nastavení synchronizace času</strong>
  </div
  <br><br>
  <form action="" method="get">
    <table border="0" style="width:310px" class="center">
      <tr>
        <td align="right">NTP Server:</td>
        <td><input type="text" id="ntpserver" name="ntpserver" maxlength="172" value=""></td>
      </tr>
      <tr>
        <td align="right">Aktualizace:</td>
        <td><input type="number" id="update" name="update" size="3" maxlength="6" value=""> minut (0=vypnutá)</td>
      </tr>
      <tr>
        <td align="right">Pásmo:</td><td>
          <select  id="tz" name="tz">
            <option value="-120">(GMT-12:00)</option>
            <option value="-110">(GMT-11:00)</option>
            <option value="-100">(GMT-10:00)</option>
            <option value="-90">(GMT-09:00)</option>
            <option value="-80">(GMT-08:00)</option>
            <option value="-70">(GMT-07:00)</option>
            <option value="-60">(GMT-06:00)</option>
            <option value="-50">(GMT-05:00)</option>
            <option value="-40">(GMT-04:00)</option>
            <option value="-35">(GMT-03:30)</option>
            <option value="-30">(GMT-03:00)</option>
            <option value="-20">(GMT-02:00)</option>
            <option value="-10">(GMT-01:00)</option>
            <option value="0">(GMT+00:00)</option>
            <option value="10">(GMT+01:00)</option>
            <option value="20">(GMT+02:00)</option>
            <option value="30">(GMT+03:00)</option>
            <option value="35">(GMT+03:30)</option>
            <option value="40">(GMT+04:00)</option>
            <option value="45">(GMT+04:30)</option>
            <option value="50">(GMT+05:00)</option>
            <option value="55">(GMT+05:30)</option>
            <option value="57">(GMT+05:45)</option>
            <option value="60">(GMT+06:00)</option>
            <option value="65">(GMT+06:30)</option>
            <option value="70">(GMT+07:00)</option>
            <option value="80">(GMT+08:00)</option>
            <option value="90">(GMT+09:00)</option>
            <option value="95">(GMT+09:30)</option>
            <option value="100">(GMT+10:00)</option>
            <option value="110">(GMT+11:00)</option>
            <option value="120">(GMT+12:00)</option>
            <option value="120">(GMT+12:00)</option>
            <option value="130">(GMT+13:00)</option>
          </select>
        </td>
      </tr>
      <tr>
        <td align="right">Letní čas:</td>
        <td><input type="checkbox" id="dst" name="dst"></td>
      </tr>
      <tr>
        <td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--green" value="Uložit"></td>
      </tr>
    </table>
  </form>

  <script>
    window.onload = function ()
    {
      load("style.css","css", function() 
      {
        load("microajax.js","js", function() 
        {
          setValues("/admin/ntpvalues");
        });
      });
    }
    function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
  </script>
)=====";

void send_NTP_configuration_html()
{
  if (server.args() > 0)  // Save Settings
  {
    config.daylight = false;
    String temp = "";
    for (uint8_t i = 0; i < server.args(); i++) {
      if (server.argName(i) == "ntpserver")
        config.ntpServerName = urlDecode(server.arg(i));
      if (server.argName(i) == "update")
        config.ntpUpdateTime = server.arg(i).toInt();
      if (server.argName(i) == "tz")
        config.timezone = server.arg(i).toInt();
      if (server.argName(i) == "dst")
        config.daylight = true;
    }
    WriteConfig();
    ntpUpdate();
  }
  server.send(200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_NTPConfiguration));
}

void send_NTP_configuration_values_html()
{
  String values = "";
  values += "ntpserver|" + (String) config.ntpServerName + "|input\n";
  values += "update|" + (String) config.ntpUpdateTime + "|input\n";
  values += "tz|" + (String) config.timezone + "|input\n";
  values += "dst|" + (String) (config.daylight ? "checked" : "") + "|chk\n";
  server.send(200, "text/plain", values);
}

#endif

