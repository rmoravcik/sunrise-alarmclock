#ifndef COMMON_H
#define COMMON_H

#include <avr/eeprom.h>

//#define DEBUG 1

#define ALARM_OFF_HOUR 99
#define ALARM_OFF_MIN 99

#define COMMAND_GET_STATUS "STAT?"
#define COMMAND_GET_STATUS_RSP "STAT+"

#define COMMAND_SET_ALARM "ALARM+"
#define COMMAND_SET_ALARM_RSP "ALARM+OK"

#define COMMAND_SET_DATE "DATE+"
#define COMMAND_SET_DATE_RSP "DATE+OK"

typedef struct {
    uint8_t hour;
    uint8_t min;
} alarm_time_t;

typedef struct {
    alarm_time_t alarm[7];
} conf_t;

#define SET_ALARM_ID_MASK 0x07
#define SET_ALARM 0x10
#define SET_DATE 0x20

#define WDAY_TO_ID(x) (x - 1)

#endif
