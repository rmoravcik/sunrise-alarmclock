#include <stdio.h>

#include "ds_rtc_lib/library-gcc/test/uart.h"
#include "light_ws2812/light_ws2812_AVR/Light_WS2812/light_ws2812.h"

#include "common.h"

#include "led.h"

#define NUM_LEDS 12

struct cRGB leds[12];

void led_init(void)
{
    led_off(0);
}

void led_sunrise(uint16_t period)
{
    uint8_t red = 0, green = 0, blue = 0;
    uint8_t i = 0;
#ifdef DEBUG
    char buf[60];
#endif

    if ((period >= 0) && (period < 600)) {
        red = ((uint32_t)period + 1) * 50 / 600;
        green = 0;
        blue = 0;
    } else if ((period >= 600) && (period < 1200)) {
        red = 50;
        green = ((uint32_t)period - 600 + 1) * 30 / 600;
        blue = 0;
    } else if ((period >= 1200) && (period < 1800)) {
        red = 50 + (((uint32_t)period - 1200 + 1) * 205 / 600);
        green = 30 + (((uint32_t)period - 1200 + 1) * 73 / 600);
        blue = ((uint32_t)period - 1200 + 1) * 23 / 600;
    } else if ((period >= 1800) && (period < 2400)) {
        red = 255;
        green = 103 + (((uint32_t)period - 1800 + 1) * 152 / 600);
        blue = 23 + (((uint32_t)period - 1800 + 1) * 232 / 600);
    } else if (period >= 2400) {
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

void led_off(uint16_t period)
{
    uint8_t i = 0;

    for (i = 0; i < NUM_LEDS; i++) {
        leds[i].r = 0;
        leds[i].g = 0;
        leds[i].b = 0;
    }

    ws2812_setleds(leds, 12);
}
