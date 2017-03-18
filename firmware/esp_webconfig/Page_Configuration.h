#ifndef PAGE_CONFIGURATION_H
#define PAGE_CONFIGURATION_H

//
//  HTML PAGE
//
const char PAGE_ConfigurationPage[] PROGMEM = R"=====(
	<meta name="viewport" content="width=device-width, initial-scale=1" />
	<a href="/" style="width:20px; text-align:center" class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Configuration</strong>
	<hr>
	<a href="network.html" style="width:250px" class="btn btn--m btn--blue">Network Configuration</a><br>
	<a href="info.html" style="width:250px" class="btn btn--m btn--blue">Network Information</a><br>
	<a href="ntp.html" style="width:250px" class="btn btn--m btn--blue">NTP Settings</a><br>

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