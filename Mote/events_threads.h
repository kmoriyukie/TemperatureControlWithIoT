// #ifndef EVENTS_
// #define EVENTS_

// #define BLINK_TON 5//seconds
// #define BLINK_INTERVAL 2//seconds
// #define N_BLINKS 4//N

// #define DEBOUNCE_TIME 0.12//seconds

// #include "stdint.h"
// #include "stdbool.h"

// void event_blink_leds(uint8_t *ret);//0brgb

// uint8_t getColor(void);

// void led_ctr(const uint8_t color);

// #endif


#ifndef EVENTS_
#define EVENTS_

#define BLINK_TON 0.5//seconds
#define BLINK_INTERVAL 1//seconds
#define N_BLINKS 5//N

#define DEBOUNCE_TIME 0.12//seconds

#include "stdint.h"
#include "stdbool.h"

void event_blink_leds(int *ret);//0brgb

uint8_t getColor(void);

void led_ctr(const uint8_t color);

#endif
