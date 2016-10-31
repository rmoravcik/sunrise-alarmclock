#include <stdio.h>

#include "rtc2.h"

static void rtc_SQW_irq_init(void)
{
    DDRC &= ~_BV(PC2);

    PCMSK1 |= _BV(PCINT10);
    PCICR |= _BV(PCIE1);
}

void rtc2_init(void)
{
    rtc_init();

    rtc_SQW_irq_init();
    rtc_SQW_enable(true);
    rtc_SQW_set_freq(FREQ_1);
}

struct tm* rtc2_get_time(void)
{
    return rtc_get_time();
}

void rtc2_set_time(struct tm* tm_)
{
    rtc_set_time(tm_);
}

void rtc2_reset_prealarm(void)
{
    rtc_set_sram_byte(0, 0); // hour
    rtc_set_sram_byte(0, 1); // minute
}

void rtc2_set_prealarm(uint8_t hour, uint8_t min)
{
    rtc_set_sram_byte(hour, 0); // hour
    rtc_set_sram_byte(min,  1); // minute
}

void rtc2_get_prealarm(uint8_t* hour, uint8_t* min)
{
    if (hour) *hour = rtc_get_sram_byte(0);
    if (min)  *min  = rtc_get_sram_byte(1);
}

bool rtc2_check_prealarm(struct tm *tm_)
{
    uint8_t hour = rtc_get_sram_byte(0);
    uint8_t min  = rtc_get_sram_byte(1);

    if (tm_->hour == hour && tm_->min == min && tm_->sec == 0)
        return true;
    return false;
}

void rtc2_reset_alarm(void)
{
    rtc_set_sram_byte(0, 2); // hour
    rtc_set_sram_byte(0, 3); // minute
}

void rtc2_set_alarm(uint8_t hour, uint8_t min)
{
    rtc_set_sram_byte(hour, 2); // hour
    rtc_set_sram_byte(min,  3); // minute
}

void rtc2_get_alarm(uint8_t* hour, uint8_t* min)
{
    if (hour) *hour = rtc_get_sram_byte(2);
    if (min)  *min  = rtc_get_sram_byte(3);
}

bool rtc2_check_alarm(struct tm *tm_)
{
    uint8_t hour = rtc_get_sram_byte(2);
    uint8_t min  = rtc_get_sram_byte(3);

    if (tm_->hour == hour && tm_->min == min && tm_->sec == 0)
        return true;
    return false;
}
