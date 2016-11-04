#include <stdio.h>

#include "ds_rtc_lib/library-gcc/test/uart.h"
#include "light_ws2812/light_ws2812_AVR/Light_WS2812/light_ws2812.h"

#include "common.h"

#include "led.h"

#define NUM_LEDS 7

#define SUNRISE_INTERVAL (TO_PERIOD(PREALARM_RUNNING_MIN) / 4)
#define SUNSET_FADEIN_INTERVAL (20)
#define SUNSET_FADEOUT_INTERVAL (20)
#define SUNSET_INTERVAL (TO_PERIOD(PREALARM_STOPPING_MIN))

struct cRGB leds[NUM_LEDS];

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

    ws2812_setleds(leds, NUM_LEDS);
}

void led_sunset(uint16_t period)
{
    static uint8_t red_init = 0, green_init = 0, blue_init = 0;
    uint8_t red = 0, green = 0, blue = 0;
    uint8_t i = 0;
#ifdef DEBUG
    char buf[60];
#endif

    if (period == 0) {
        red_init = leds[0].r;
        green_init = leds[0].g;
        blue_init = leds[0].b;
    }

    if ((period >= 0) && (period < SUNSET_FADEIN_INTERVAL)) {
        red = red_init + (((uint32_t)period + 1) * (255 - red_init) / SUNSET_FADEIN_INTERVAL);
        green = green_init + (((uint32_t)period + 1) * (255 - green_init) / SUNSET_FADEIN_INTERVAL);
        blue = blue_init + (((uint32_t)period + 1) * (255 - blue_init) / SUNSET_FADEIN_INTERVAL);
    } else if ((period >= 3) && (period <= (SUNSET_INTERVAL - SUNSET_FADEIN_INTERVAL - SUNSET_FADEOUT_INTERVAL))) {
        red = 255;
        green = 255;
        blue = 255;
    } else {
        red = 255 - (((uint32_t)period + 1) * 255 / SUNSET_FADEOUT_INTERVAL);
        green = 255 - (((uint32_t)period + 1) * 255 / SUNSET_FADEOUT_INTERVAL);
        blue = 255 - (((uint32_t)period + 1) * 255 / SUNSET_FADEOUT_INTERVAL);
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

    ws2812_setleds(leds, NUM_LEDS);
}

void led_on(void)
{
    uint8_t i = 0;

    for (i = 0; i < NUM_LEDS; i++) {
        leds[i].r = 255;
        leds[i].g = 255;
        leds[i].b = 255;
    }

    ws2812_setleds(leds, NUM_LEDS);
}

void led_off(void)
{
    uint8_t i = 0;

    for (i = 0; i < NUM_LEDS; i++) {
        leds[i].r = 0;
        leds[i].g = 0;
        leds[i].b = 0;
    }

    ws2812_setleds(leds, NUM_LEDS);
}
