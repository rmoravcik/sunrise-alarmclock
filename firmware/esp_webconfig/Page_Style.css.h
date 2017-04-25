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

const char PAGE_Style_css[] PROGMEM = R"=====(
body {
  color: #000000;
  font-family: avenir, helvetica, arial, sans-serif;
  letter-spacing: 0.15em;
  margin: 0;
}

hr {
  background-color: #eee;
  border: 0 none;
  color: #eee;
  height: 1px;
}

table {
  background-color: #f1f1f1;
}

table.center {
  margin-left:auto;
  margin-right:auto;
}

td {
  padding: 12px;
}

input[type="number"] {
    width: 50px;
}

#menu {
  margin: 0px;
  padding: 0px;
  padding-top: 5px;
  color: #f1f1f1;
  background-color: #5f5f5f;
  white-space: nowrap;
}

.btn, .btn:link, .btn:visited {
  border-radius: 0.3em;
  border-style: solid;
  border-width: 1px;
  color: #f1f1f1;
  display: inline-block;
  font-family: avenir, helvetica, arial, sans-serif;
  letter-spacing: 0.15em;
  margin-bottom: 0.3em;
  padding: 1em 0.75em;
  text-decoration: none;
  text-transform: uppercase;
  -webkit-transition: color 0.4s, background-color 0.4s, border 0.4s;
  transition: color 0.4s, background-color 0.4s, border 0.4s;
}

.btn--green, .btn--green:link, .btn-green:visited {
  background-color: #4caf50;
}

@media screen and (min-width: 32em) {
  .btn--full {
    max-width: 16em !important;
  }
}
)=====";
