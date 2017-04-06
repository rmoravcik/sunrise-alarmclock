#ifndef HELPERS_H
#define HELPERS_H

bool checkRange(String Value);

String getMacAddress();

String urlDecode(String input);

#if 0

static const uint8_t monthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
#define LEAP_YEAR(Y) ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )


boolean isSummerTime(int year, byte month, byte day, byte hour, byte tzHours)
// input parameters: "normal time" for year, month, day, hour and tzHours (0=UTC, 1=MEZ)
{
  if (month < 3 || month > 10)
    return false;
  if (month > 3 && month < 10)
    return true;
  if (((month == 3) && ((hour + 24 * day) >= (1 + tzHours + 24 * (31 - (5 * year / 4 + 4) % 7)))) ||
      ((month == 10) && ((hour + 24 * day) < (1 + tzHours + 24 * (31 - (5 * year / 4 + 1) % 7)))))
    return true;
  else
    return false;
}

//
// Check the Values is between 0-255
//

void ConvertUnixTimeStamp(unsigned long TimeStamp, struct strDateTime *DateTime)
{
  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;
  time = (uint32_t) TimeStamp;
  DateTime->second = time % 60;
  time /= 60;    // now it is minutes
  DateTime->minute = time % 60;
  time /= 60;    // now it is hours
  DateTime->hour = time % 24;
  time /= 24;    // now it is days
  DateTime->wday = ((time + 4) % 7) + 1;  // Sunday is day 1 

  year = 0;
  days = 0;
  while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  DateTime->year = year;  // year is offset from 1970 

  days -= LEAP_YEAR(year) ? 366 : 365;
  time -= days;    // now it is days in this year, starting at 0

  days = 0;
  month = 0;
  monthLength = 0;
  for (month = 0; month < 12; month++) {
    if (month == 1) {  // february
      if (LEAP_YEAR(year)) {
        monthLength = 29;
      } else {
        monthLength = 28;
      }
    } else {
      monthLength = monthDays[month];
    }

    if (time >= monthLength) {
      time -= monthLength;
    } else {
      break;
    }
  }
  DateTime->month = month + 1;  // jan is month 1  
  DateTime->day = time + 1;  // day of month
  DateTime->year += 1970;

}


boolean firstStart = true;  // On firststart = true, NTP will try to get a valid time
WiFiUDP UDPNTPClient;   // NTP Client
unsigned long UnixTimestamp = 0;  // GLOBALTIME  ( Will be set by NTP)
int cNTP_Update = 0;    // Counter for Updating the time via NTP
byte Minute_Old = 100;    // Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)


/*
**
**  NTP
**
*/
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

void NTPRefresh()
{
  if (WiFi.status() == WL_CONNECTED) {
    IPAddress timeServerIP;

    WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP);
    //sendNTPpacket(timeServerIP); // send an NTP packet to a time server

    Serial.println("sending NTP packet...");

    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;  // Stratum, or type of clock
    packetBuffer[2] = 6;  // Polling Interval
    packetBuffer[3] = 0xEC; // Peer Clock Precision
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    UDPNTPClient.beginPacket(timeServerIP, 123);
    UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
    UDPNTPClient.endPacket();

    delay(1000);

    int cb = UDPNTPClient.parsePacket();
    if (!cb) {
      Serial.println("NTP no packet yet");
    } else {
      Serial.print("NTP packet received, length=");
      Serial.println(cb);
      UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
      unsigned long highWord =
          word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord =
          word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears;
      UnixTimestamp = epoch;
    }
  }
}
#endif

#endif
