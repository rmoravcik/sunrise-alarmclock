#ifndef PAGE_CONFIGURATION_H
#define PAGE_CONFIGURATION_H

//
//  HTML PAGE
//
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
