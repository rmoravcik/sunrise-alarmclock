#ifndef PAGE_ROOT_H
#define PAGE_ROOT_H

//
//   The ROOT PAGE
//
const char PAGE_root[] PROGMEM = R"=====(
	<meta name="viewport" content="width=device-width, initial-scale=1" />
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<h1>My Example goes here</h1>
	<div id="mydynamicdata">Here comes the Dynamic Data in </div>   <!-- added a DIV, where the dynamic data goes to -->
	<script>
		window.onload = function ()
		{
			load("style.css","css", function() 
			{
				load("microajax.js","js", function() 
				{
						setValues("/admin/rootvalues");  //-- this function calls the function on the ESP      
				});
			});
		}
		function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
	</script>
)=====";

void send_root_values_html()
{
	String values ="";
	values += "mydynamicdata|" + (String) + "This is filled by AJAX. Millis since start: " + (String) millis() + "|div\n";   // Build a string, like this:  ID|VALUE|TYPE
	server.send(200, "text/plain", values);
  Serial.println(__FUNCTION__);
}

void send_root_html()
{
	if (server.args() > 0)  // Are there any POST/GET Fields ?
	{
		for (uint8_t i = 0; i < server.args(); i++) {  // Iterate through the fields
			if (server.argName(i) == "firstname")
			{
				// Your processing for the transmitted form-variable 
				String fName = server.arg(i);
			}
		}
	}

	server.send(200, "text/html", reinterpret_cast<const __FlashStringHelper *>(PAGE_root));
  Serial.println(__FUNCTION__);
}
#endif
