#ifndef COMMON_H
#define COMMON_H

#include <avr/eeprom.h>

#define DEBUG 1

#ifdef DEBUG
extern uint8_t debug;

#define DEBUG_OFF	0x00
#define DEBUG_RTC	0x01
#define DEBUG_COMMAND	0x02
#define DEBUG_LED	0x04
#define DEBUG_FSM	0x08
#endif

#define ALARM_OFF_HOUR	99
#define ALARM_OFF_MIN	99

#define PREALARM_RUNNING_MIN	20
#define PREALARM_STOPPING_MIN	5
#define ALARM_RUNNING_MIN	1
#define ALARM_STOPPING_MIN	5

#define COMMAND_PING "PING?"
#define COMMAND_PING_RSP "PONG+OK"

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

#define SET_ALARM_ID_MASK	0x0007
#define SET_ALARM		0x0010
#define SET_DATE		0x0020
#define PREALARM_RUNNING	0x0040
#define PREALARM_STOPPING	0x0080
#define ALARM_RUNNING		0x0100
#define ALARM_STOPPING		0x0200
#define ALARM_STOP_REQUEST	0x0400

#define WDAY_TO_ID(x) (x - 1)

#endif
