#include "light_ws2812/light_ws2812_AVR/Light_WS2812/light_ws2812.h"

#include "led.h"

struct cRGB led[12];

void led_init(void)
{
    led[0].r=255;led[0].g=00;led[0].b=0;
//    led[1].r=255;led[1].g=00;led[1].b=0;
//    led[2].r=255;led[2].g=00;led[2].b=0;
    led[11].r=255;led[11].g=00;led[11].b=0;
    ws2812_setleds(led,12);
}
