#include "light_ws2812/light_ws2812_AVR/Light_WS2812/light_ws2812.h"

#include "led.h"

#define NUM_LEDS 2

struct cRGB leds[12];

void led_init(void)
{
    uint8_t i = 0;

    for (i = 0; i < NUM_LEDS; i++) {
        leds[i].r = 0;
        leds[i].g = 0;
        leds[i].b = 0;
    }

    ws2812_setleds(leds, 12);
}

void led_sunrise(uint16_t period)
{
    uint8_t red = 0, green = 0, blue = 0;
    uint8_t i = 0;

    if ((period >= 0) && (period < 600)) {
        red = (period + 1) * 255 / 600;
        green = 0;
        blue = 0;
    } else if ((period >= 600) && (period < 1200)) {
        red = 255;
        green = (period - 600 + 1) * 160 / 600;
        blue = 0;
    } else if ((period >= 1200) && (period < 1800)) {
        red = 255;
        green = 160 + ((period - 1200 + 1) * 95 / 600);
        blue = 0;
    } else if ((period >= 1800) && (period < 2400)) {
        red = 255;
        green = 255;
        blue = (period - 1800 + 1) * 255 / 600;
    }

    for (i = 0; i < NUM_LEDS; i++) {
        leds[i].r = red;
        leds[i].g = green;
        leds[i].b = blue;
    }

    ws2812_setleds(leds, 12);
}
