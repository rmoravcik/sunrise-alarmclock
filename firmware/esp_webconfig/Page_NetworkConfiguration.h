#ifndef PAGE_NETWORKCONFIGURATION
#define PAGE_NETWORKCONFIGURATION

#include "config.h"
#include "common.h"

const char PAGE_NetworkConfiguration[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <link rel="stylesheet" href="style.css" type="text/css" />
  <script src="microajax.js"></script>

  <div id="menu">
    &nbsp;<a href="config.html" style="width:25px; text-align:center;font-weight:bold" class="btn"><</a>&nbsp;&nbsp;<strong>Nastavení síťe</strong>
  </div
  <br><br>
  <form action="" method="get">
    <table border="0" style="width:310px" class="center">
      <tr>
        <td align="right">Jméno:</td>
        <td><input type="text" id="hostname" name="hostname" value=""></td>
      </tr>
      <tr>
        <td align="right">SSID:</td>
        <td><input type="text" id="ssid" name="ssid" value=""></td>
      </tr>
      <tr>
        <td align="right">Heslo:</td>
        <td><input type="text" id="password" name="password" value=""></td>
      </tr>
      <tr>
        <td align="right">DHCP:</td>
        <td><input type="checkbox" id="dhcp" name="dhcp"></td>
      </tr>
      <tr>
        <td align="right">IP:</td>
        <td><input type="number" id="ip_0" name="ip_0" size="3">.<input type="number" id="ip_1" name="ip_1" size="3">.<input type="number" id="ip_2" name="ip_2" size="3">.<input type="number" id="ip_3" name="ip_3" value="" size="3"></td>
      </tr>
      <tr>
        <td align="right">Maska:</td>
        <td><input type="number" id="nm_0" name="nm_0" size="3">.<input type="number" id="nm_1" name="nm_1" size="3">.<input type="number" id="nm_2" name="nm_2" size="3">.<input type="number" id="nm_3" name="nm_3" size="3"></td>
      </tr>
      <tr>
        <td align="right">Brána:</td>
        <td><input type="number" id="gw_0" name="gw_0" size="3">.<input type="number" id="gw_1" name="gw_1" size="3">.<input type="number" id="gw_2" name="gw_2" size="3">.<input type="number" id="gw_3" name="gw_3" size="3"></td>
      </tr>
      <tr>
        <td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--green" value="Uložit"></td>
      </tr>
    </table>
  </form>
  <table border="0" style="width:310px" class="center">
    <tr>
      <td style="padding:0px"><div id="networks"><span style="padding:12px">Hledám sítě...</span></div></td>
    </tr>
    <tr>
      <td align="center"><a href="javascript:GetState()" style="width:150px" class="btn btn--m btn--green">Obnovit</a></td>
    </tr>
  </table>

  <script>
    function GetState()
    {
      setValues("/admin/connectionstate");
    }
    function selssid(value)
    {
      document.getElementById("ssid").value = value; 
    }
    window.onload = function ()
    {
      load("style.css","css", function() 
      {
        load("microajax.js","js", function() 
        {
          setValues("/admin/networkvalues");
          setTimeout(GetState,3000);
        });
      });
    }
    function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
  </script>
)=====";

const char PAGE_WaitAndReload[] PROGMEM = R"=====(
  <meta http-equiv="refresh" content="5; URL=config.html">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  Počkejte prosím.... Konfiguruji a restartuji.
)=====";

void send_network_configuration_html()
{
  if (server.args() > 0)
  {
    String temp = "";

    config.dhcpEnabled = false;
    
    for (uint8_t i = 0; i < server.args(); i++) {
      if (server.argName(i) == "hostname")
        config.hostname = urlDecode(server.arg(i));
      if (server.argName(i) == "ssid")
        config.ssid = urlDecode(server.arg(i));
      if (server.argName(i) == "password")
        config.password = urlDecode(server.arg(i));
      if (server.argName(i) == "ip_0")
        if (checkRange(server.arg(i)))
          config.ipAddress[0] = server.arg(i).toInt();
      if (server.argName(i) == "ip_1")
        if (checkRange(server.arg(i)))
          config.ipAddress[1] = server.arg(i).toInt();
      if (server.argName(i) == "ip_2")
        if (checkRange(server.arg(i)))
          config.ipAddress[2] = server.arg(i).toInt();
      if (server.argName(i) == "ip_3")
        if (checkRange(server.arg(i)))
          config.ipAddress[3] = server.arg(i).toInt();
      if (server.argName(i) == "nm_0")
        if (checkRange(server.arg(i)))
          config.netmask[0] =
              server.arg(i).toInt();
      if (server.argName(i) == "nm_1")
        if (checkRange(server.arg(i)))
          config.netmask[1] =
              server.arg(i).toInt();
      if (server.argName(i) == "nm_2")
        if (checkRange(server.arg(i)))
          config.netmask[2] =
              server.arg(i).toInt();
      if (server.argName(i) == "nm_3")
        if (checkRange(server.arg(i)))
          config.netmask[3] =
              server.arg(i).toInt();
      if (server.argName(i) == "gw_0")
        if (checkRange(server.arg(i)))
          config.gateway[0] =
              server.arg(i).toInt();
      if (server.argName(i) == "gw_1")
        if (checkRange(server.arg(i)))
          config.gateway[1] =
              server.arg(i).toInt();
      if (server.argName(i) == "gw_2")
        if (checkRange(server.arg(i)))
          config.gateway[2] =
              server.arg(i).toInt();
      if (server.argName(i) == "gw_3")
        if (checkRange(server.arg(i)))
          config.gateway[3] =
              server.arg(i).toInt();
      if (server.argName(i) == "dhcp")
        config.dhcpEnabled = true;
    }

    server.send(200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_WaitAndReload));

    WriteConfig();
    ConfigureNetwork();
  } else {
    server.send(200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_NetworkConfiguration));
  }
}

void send_network_configuration_values_html()
{
  String values = "";

  values += "hostname|" + (String) config.hostname + "|input\n";
  values += "ssid|" + (String) config.ssid + "|input\n";
  values += "password|" + (String) config.password + "|input\n";
  values += "ip_0|" + (String) config.ipAddress[0] + "|input\n";
  values += "ip_1|" + (String) config.ipAddress[1] + "|input\n";
  values += "ip_2|" + (String) config.ipAddress[2] + "|input\n";
  values += "ip_3|" + (String) config.ipAddress[3] + "|input\n";
  values += "nm_0|" + (String) config.netmask[0] + "|input\n";
  values += "nm_1|" + (String) config.netmask[1] + "|input\n";
  values += "nm_2|" + (String) config.netmask[2] + "|input\n";
  values += "nm_3|" + (String) config.netmask[3] + "|input\n";
  values += "gw_0|" + (String) config.gateway[0] + "|input\n";
  values += "gw_1|" + (String) config.gateway[1] + "|input\n";
  values += "gw_2|" + (String) config.gateway[2] + "|input\n";
  values += "gw_3|" + (String) config.gateway[3] + "|input\n";
  values += "dhcp|" + (String) (config.dhcpEnabled ? "checked" : "") + "|chk\n";

  server.send(200, "text/plain", values);
}

void send_connection_state_values_html()
{
  String Networks = "";

  int n = WiFi.scanNetworks();

  if (n == 0) {
    Networks = "<font color='#FF0000'>Nenalezeny žádné sítě!</font>";
  } else {
    Networks += "<table border='0' style='width:310px' class='center'>";
    Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Název</strong></td><td><strong>Kvalita</strong></td><td><strong>Šif.</strong></td><tr>";
    for (int i = 0; i < n; ++i) {
      int quality = 0;
      if (WiFi.RSSI(i) <= -100) {
        quality = 0;
      } else if (WiFi.RSSI(i) >= -50) {
        quality = 100;
      } else {
        quality = 2 * (WiFi.RSSI(i) + 100);
      }

      Networks += "<tr><td><a href='javascript:selssid(\"" +
                  String(WiFi.SSID(i)) + "\")'>" +
                  String(WiFi.SSID(i)) + "</a></td><td>" +
                  String(quality) + "%</td><td>" +
                  String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*") + "</td></tr>";
    }
    Networks += "</table>";
  }

  String values = "";

  values += "networks|" + Networks + "|div\n";

  server.send(200, "text/plain", values);
}

#endif

