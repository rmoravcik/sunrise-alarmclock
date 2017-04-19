#include "config.h"
#include "common.h"
#include "ntp.h"

DateTime dateTime;

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

int ntpUpdateTimeout = 0;
bool waitingNtpResponse = false;

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

void ConvertUnixTimeStamp(unsigned long timestamp, struct DateTime *dt)
{
  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;
  
  time = (uint32_t) timestamp;
  dt->second = time % 60;
  time /= 60;    // now it is minutes
  dt->minute = time % 60;
  time /= 60;    // now it is hours
  dt->hour = time % 24;
  time /= 24;    // now it is days
  dt->wday = ((time + 4) % 7) + 1;  // Sunday is day 1 

  year = 0;
  days = 0;
  while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }

  dt->year = year;  // year is offset from 1970 

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

  dt->month = month + 1;  // jan is month 1  
  dt->day = time + 1;  // day of month
  dt->year += 1970;
}

void ntpUpdate(void)
{
  if (config.ntpUpdateTime > 0) {
#ifdef SERIAL_DEBUG  
    Serial.print("Sending NTP request to ");
    Serial.println(config.ntpServerName.c_str());
#endif

    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;  // Stratum, or type of clock
    packetBuffer[2] = 6;  // Polling Interval
    packetBuffer[3] = 0xEC; // Peer Clock Precision
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    udp.beginPacket(config.ntpServerName.c_str(), 123);
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();

    waitingNtpResponse = true;
    ntpUpdateTimeout = 0;
  }
}

