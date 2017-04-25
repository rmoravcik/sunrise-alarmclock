/*
 *  sunrise-alarmclock
 *  Copyright (C) 2016,2017 Roman Moravcik
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef RTC_WRAPPER_H
#define RTC_WRAPPER_H

#include "ds_rtc_lib/library-gcc/rtc.h"

void rtc_wrapper_init(void);

struct tm* rtc_wrapper_get_time(void);
void rtc_wrapper_set_time(struct tm* tm_);

// Alarm functionality
void rtc_wrapper_reset_prealarm(void);
void rtc_wrapper_set_prealarm(uint8_t hour, uint8_t min);
void rtc_wrapper_get_prealarm(uint8_t* hour, uint8_t* min);
bool rtc_wrapper_check_prealarm(struct tm *tm_);

void rtc_wrapper_reset_alarm(void);
void rtc_wrapper_set_alarm(uint8_t hour, uint8_t min);
void rtc_wrapper_get_alarm(uint8_t* hour, uint8_t* min);
bool rtc_wrapper_check_alarm(struct tm *tm_);

#endif
