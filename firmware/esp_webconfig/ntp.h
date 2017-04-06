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

extern DateTime dateTime;

#endif
