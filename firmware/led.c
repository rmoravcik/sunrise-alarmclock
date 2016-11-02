#include <stdio.h>

#include "ds_rtc_lib/library-gcc/test/uart.h"
#include "light_ws2812/light_ws2812_AVR/Light_WS2812/light_ws2812.h"

#include "common.h"

#include "led.h"

#define NUM_LEDS 12

#define SUNRISE_INTERVAL (PREALARM_RUNNING_MIN * 120 / 4)
#define SUNSET_INTERVAL (PREALARM_STOPPING_MIN * 120)

struct cRGB leds[12];

void led_init(void)
{
    led_off();
}

void led_sunrise(uint16_t period)
{
    uint8_t red = 0, green = 0, blue = 0;
    uint8_t i = 0;
#ifdef DEBUG
    char buf[60];
#endif

    if ((period >= 0) && (period < SUNRISE_INTERVAL)) {
        red = ((uint32_t)period + 1) * 50 / SUNRISE_INTERVAL;
        green = 0;
        blue = 0;
    } else if ((period >= SUNRISE_INTERVAL) && (period < (2 * SUNRISE_INTERVAL))) {
        red = 50;
        green = ((uint32_t)period - SUNRISE_INTERVAL + 1) * 30 / SUNRISE_INTERVAL;
        blue = 0;
    } else if ((period >= (2 * SUNRISE_INTERVAL)) && (period < (3 * SUNRISE_INTERVAL))) {
        red = 50 + (((uint32_t)period - (2 * SUNRISE_INTERVAL) + 1) * 205 / SUNRISE_INTERVAL);
        green = 30 + (((uint32_t)period - (2 * SUNRISE_INTERVAL) + 1) * 73 / SUNRISE_INTERVAL);
        blue = ((uint32_t)period - (2 * SUNRISE_INTERVAL) + 1) * 23 / SUNRISE_INTERVAL;
    } else if ((period >= (3 * SUNRISE_INTERVAL)) && (period < (4 * SUNRISE_INTERVAL))) {
        red = 255;
        green = 103 + (((uint32_t)period - (3 * SUNRISE_INTERVAL) + 1) * 152 / SUNRISE_INTERVAL);
        blue = 23 + (((uint32_t)period - (3 * SUNRISE_INTERVAL) + 1) * 232 / SUNRISE_INTERVAL);
    } else if (period >= (4 * SUNRISE_INTERVAL)) {
        red = 255;
        green = 255;
        blue = 255;
    }

#ifdef DEBUG
    if (debug & DEBUG_LED) {
        sprintf(buf, "led_sunrise: period=%4u, rgb(%03u,%03u,%03u)\r\n",
                period, red, green, blue);
        uartSendString(buf);
    }
#endif

    for (i = 0; i < NUM_LEDS; i++) {
        leds[i].r = red;
        leds[i].g = green;
        leds[i].b = blue;
    }

    ws2812_setleds(leds, 12);
}

void led_sunset(uint16_t period)
{
    uint8_t red = 0, green = 0, blue = 0;
    uint8_t i = 0;
#ifdef DEBUG
    char buf[60];
#endif

    if ((period >= 0) && (period <= SUNSET_INTERVAL)) {
        red = 255 - (((uint32_t)period + 1) * 255 / SUNSET_INTERVAL);
        green = 255 - (((uint32_t)period + 1) * 255 / SUNSET_INTERVAL);
        blue = 255 - (((uint32_t)period + 1) * 255 / SUNSET_INTERVAL);
    }

#ifdef DEBUG
    if (debug & DEBUG_LED) {
        sprintf(buf, "led_sunset: period=%4u, rgb(%03u,%03u,%03u)\r\n",
                period, red, green, blue);
        uartSendString(buf);
    }
#endif

    for (i = 0; i < NUM_LEDS; i++) {
        leds[i].r = red;
        leds[i].g = green;
        leds[i].b = blue;
    }

    ws2812_setleds(leds, 12);
}

void led_on(void)
{
    uint8_t i = 0;

    for (i = 0; i < NUM_LEDS; i++) {
        leds[i].r = 255;
        leds[i].g = 255;
        leds[i].b = 255;
    }

    ws2812_setleds(leds, 12);
}

void led_off(void)
{
    uint8_t i = 0;

    for (i = 0; i < NUM_LEDS; i++) {
        leds[i].r = 0;
        leds[i].g = 0;
        leds[i].b = 0;
    }

    ws2812_setleds(leds, 12);
}
