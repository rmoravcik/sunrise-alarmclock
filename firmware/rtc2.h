#ifndef RTC2_H
#define RTC2_H

#include "ds_rtc_lib/library-gcc/rtc.h"

void rtc2_init(void);

struct tm* rtc2_get_time(void);
void rtc2_set_time(struct tm* tm_);

// Alarm functionality
void rtc2_reset_prealarm(void);
void rtc2_set_prealarm(uint8_t hour, uint8_t min);
void rtc2_get_prealarm(uint8_t* hour, uint8_t* min);
bool rtc2_check_prealarm(struct tm *tm_);

void rtc2_reset_alarm(void);
void rtc2_set_alarm(uint8_t hour, uint8_t min);
void rtc2_get_alarm(uint8_t* hour, uint8_t* min);
bool rtc2_check_alarm(struct tm *tm_);

#endif
