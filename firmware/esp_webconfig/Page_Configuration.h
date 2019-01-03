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

#ifndef PAGE_CONFIGURATION_H
#define PAGE_CONFIGURATION_H

const char PAGE_ConfigurationPage[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <link rel="stylesheet" href="style.css" type="text/css" />
  <script src="microajax.js"></script>

  <div id="menu">
    &nbsp;<a href="/" style="width:25px; text-align:center;font-weight:bold" class="btn"><</a>&nbsp;&nbsp;<strong>Nastavení</strong>
  </div
  <br><br>
  <table border="0" style="width:310px" class="center">
    <tr>
      <td colspan="1" align="center"><a href="network.html" style="width:250px;text-align:center;" class="btn btn--m btn--green">Nastavení síťe</a></td>
    </tr>
    <tr>
      <td colspan="1" align="center"><a href="info.html" style="width:250px;text-align:center;" class="btn btn--m btn--green">Informace o síti</a></td>
    </tr>
    <tr>
      <td colspan="1" align="center"><a href="ntp.html" style="width:250px" class="btn btn--m btn--green">Nastavení synchronizace času</a></td>
    </tr>
#ifdef ENABLED_LOGGING
    <tr>
      <td colspan="1" align="center"><a href="log.html" style="width:250px" class="btn btn--m btn--green">Záznam</a></td>
    </tr>
#endif
  </table>

  <script>
    window.onload = function ()
    {
      load("style.css","css", function()
      {
        load("microajax.js","js", function()
        {
          // Do something after load...
        });
      });
    }
    function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
  </script>
)=====";

#endif
