#ifndef NTP_H
#define NTP_H

#include <Arduino.h>

struct DateTime {
  byte hour;
  byte minute;
  byte second;
  int year;
  byte month;
  byte day;
  byte wday;
};

extern unsigned long utcTime;

extern int ntpUpdateTimeout;
extern bool waitingNtpResponse;

bool IsSummerTime(int year, byte month, byte day, byte hour, long tz);

void EpochToDateTime(unsigned long epoch, struct DateTime *dt);
void LocalTime(unsigned long epoch, struct DateTime *dt);

void ntpUpdate(void);
void ntpResponse(void);

#endif
