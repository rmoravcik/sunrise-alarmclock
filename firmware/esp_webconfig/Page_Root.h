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

#ifndef PAGE_ROOT_H
#define PAGE_ROOT_H

#include "WString.h"

#include "config.h"
#include "helpers.h"
#include "common.h"

const char PAGE_root[] PROGMEM = R"=====(
    <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <link rel="stylesheet" href="style.css" type="text/css" />
    <script src="microajax.js"></script>

  <div id="menu">
    &nbsp;<a href="config.html" style="width:25px; text-align:center;font-weight:bold" class="btn">⋮</a>&nbsp;&nbsp;<strong>Budík</strong>
  </div
  <br><br>
  <form action="" method="get">
  <table border="0" style="width:310px" class="center">
      <tr>
        <td align="left">Pondělí</td>
        <td><input type="time" id="day1" name="day1" value=""></td>
        <td><input type="checkbox" id="day1_enable" name="day1_enable"></td>
      </tr>
      <tr>
        <td align="left">Úterý</td>
        <td><input type="time" id="day2" name="day2" value=""></td>
        <td><input type="checkbox" id="day2_enable" name="day2_enable"></td>
      </tr>
      <tr>
        <td align="left">Středa</td>
        <td><input type="time" id="day3" name="day3" value=""></td>
        <td><input type="checkbox" id="day3_enable" name="day3_enable"></td>
      </tr>
      <tr>
        <td align="left">Čtvrtek</td>
        <td><input type="time" id="day4" name="day4" value=""></td>
        <td><input type="checkbox" id="day4_enable" name="day4_enable"></td>
      </tr>
      <tr>
        <td align="left">Pátek</td>
        <td><input type="time" id="day5" name="day5" value=""></td>
        <td><input type="checkbox" id="day5_enable" name="day5_enable"></td>
      </tr>
      <tr>
        <td align="left">Sobota</td>
        <td><input type="time" id="day6" name="day6" value=""></td>
        <td><input type="checkbox" id="day6_enable" name="day6_enable"></td>
      </tr>
      <tr>
        <td align="left">Neděle</td>
        <td><input type="time" id="day7" name="day7" value=""></td>
        <td><input type="checkbox" id="day7_enable" name="day7_enable"></td>
      </tr>
      <tr>
        <td colspan="3" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--green" value="Nastavit"></td>
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
          setValues("/admin/rootvalues");
        });
      });
    }
    function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
  </script>
)=====";

void send_root_values_html()
{
  String values ="";

  SendGetStatusCommand();

  for (int i = 0; i < 7; i++) {
    char time_buf[6];
    sprintf(time_buf, "%02d:%02d", config.alarm[i].hour, config.alarm[i].minute);
    values += "day"+ (String) (i + 1) + "|" + (String) time_buf + "|input\n";
    values += "day"+ (String) (i + 1) + "_enable|" + (String) (config.alarm[i].enabled ? "checked" : "") + "|chk\n";
  }

  server.send(200, "text/plain", values);
}

void send_root_html()
{
  if (server.args() > 0)  // Are there any POST/GET Fields ?
  {
    for (int i = 0; i < 7; i++) {
      config.alarm[i].enabled = false;
    }

    for (uint8_t i = 0; i < server.args(); i++) {  // Iterate through the fields
      if (server.argName(i) == "day1")
      {
        if (checkTime(server.arg(i))) {
          config.alarm[0].hour = server.arg(i).substring(0,2).toInt();
          config.alarm[0].minute = server.arg(i).substring(3,5).toInt();
        }
      }
      if (server.argName(i) == "day1_enable")
      {
        config.alarm[0].enabled = true;
      }
      if (server.argName(i) == "day2")
      {
        if (checkTime(server.arg(i))) {
          config.alarm[1].hour = server.arg(i).substring(0,2).toInt();
          config.alarm[1].minute = server.arg(i).substring(3,5).toInt();
        }
      }
      if (server.argName(i) == "day2_enable")
      {
        config.alarm[1].enabled = true;
      }
      if (server.argName(i) == "day3")
      {
        if (checkTime(server.arg(i))) {
          config.alarm[2].hour = server.arg(i).substring(0,2).toInt();
          config.alarm[2].minute = server.arg(i).substring(3,5).toInt();
        }
      }
      if (server.argName(i) == "day3_enable")
      {
        config.alarm[2].enabled = true;
      }
      if (server.argName(i) == "day4")
      {
        if (checkTime(server.arg(i))) {
          config.alarm[3].hour = server.arg(i).substring(0,2).toInt();
          config.alarm[3].minute = server.arg(i).substring(3,5).toInt();
        }
      }
      if (server.argName(i) == "day4_enable")
      {
        config.alarm[3].enabled = true;
      }
      if (server.argName(i) == "day5")
      {
        if (checkTime(server.arg(i))) {
          config.alarm[4].hour = server.arg(i).substring(0,2).toInt();
          config.alarm[4].minute = server.arg(i).substring(3,5).toInt();
        }
      }
      if (server.argName(i) == "day5_enable")
      {
        config.alarm[4].enabled = true;
      }
      if (server.argName(i) == "day6")
      {
        if (checkTime(server.arg(i))) {
          config.alarm[5].hour = server.arg(i).substring(0,2).toInt();
          config.alarm[5].minute = server.arg(i).substring(3,5).toInt();
        }
      }
      if (server.argName(i) == "day6_enable")
      {
        config.alarm[5].enabled = true;
      }
      if (server.argName(i) == "day7")
      {
        if (checkTime(server.arg(i))) {
          config.alarm[6].hour = server.arg(i).substring(0,2).toInt();
          config.alarm[6].minute = server.arg(i).substring(3,5).toInt();
        }
      }
      if (server.argName(i) == "day7_enable")
      {
        config.alarm[6].enabled = true;
      }
    }

    WriteConfig();

    for (int i = 0; i < 7; i++) {
      SendSetAlarmCommand(i);
    }
  }

  server.send(200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_root));
}

#endif
