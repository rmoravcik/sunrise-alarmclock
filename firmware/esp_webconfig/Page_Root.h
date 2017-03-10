#ifndef PAGE_ROOT_H
#define PAGE_ROOT_H

//
//   The ROOT PAGE
//
const char PAGE_root[] PROGMEM = R"=====(
	<meta name="viewport" content="width=device-width, initial-scale=1" />
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<link rel="stylesheet" href="style.css" type="text/css" />
	<script src="microajax.js"></script>
	&nbsp;&nbsp;<strong>Sunrise Alarm</strong><a href="config.html" class="btn btn--s right">⋮</a>
	<hr>
	<form action="" method="get">
	<table border="0" cellspacing="0" cellpadding="3" style="width:310px">
			<tr>
				<td align="left">Monday</td>
				<td><input type="time" id="day1" name="day1" value="00:00"></td>
				<td><input type="checkbox" id="day1_enable" name="day1_enable"></td>
			</tr>
			<tr>
				<td align="left">Tuesday</td>
				<td><input type="time" id="day2" name="day2" value="00:00"></td>
				<td><input type="checkbox" id="day2_enable" name="day2_enable"></td>
			</tr>
			<tr>
				<td align="left">Wednesday</td>
				<td><input type="time" id="day3" name="day3" value="00:00"></td>
				<td><input type="checkbox" id="day3_enable" name="day3_enable"></td>
			</tr>
			<tr>
				<td align="left">Thursday</td>
				<td><input type="time" id="day4" name="day4" value="00:00"></td>
				<td><input type="checkbox" id="day4_enable" name="day4_enable"></td>
			</tr>
			<tr>
				<td align="left">Friday</td>
				<td><input type="time" id="day5" name="day5" value="00:00"></td>
				<td><input type="checkbox" id="day5_enable" name="day5_enable"></td>
			</tr>
			<tr>
				<td align="left">Saturday</td>
				<td><input type="time" id="day6" name="day6" value="00:00"></td>
				<td><input type="checkbox" id="day6_enable" name="day6_enable"></td>
			</tr>
			<tr>
				<td align="left">Sunday</td>
				<td><input type="time" id="day7" name="day7" value="00:00"></td>
				<td><input type="checkbox" id="day7_enable" name="day7_enable"></td>
			</tr>
		</table>
	</form>

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
