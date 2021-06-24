#ifndef EVENTS_
#define EVENTS_

#define BLINK_TON 0.5//seconds
#define BLINK_INTERVAL 2//seconds
#define N_BLINKS 10//seconds

#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"

void event_blink_leds(uint8_t color);//0brgb

void led_ctr(bool state,const uint8_t color);

#endif
