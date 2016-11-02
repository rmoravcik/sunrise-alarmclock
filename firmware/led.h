#ifndef LED_H
#define LED_H

void led_init(void);

void led_sunrise(uint16_t period);
void led_on(void);
void led_off(uint16_t period);

#endif