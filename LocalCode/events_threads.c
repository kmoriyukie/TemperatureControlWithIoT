#include "events_threads.h"

#include "contiki.h"
#include "stdio.h"

#include "sys/etimer.h"
#include "dev/leds.h"

// static bool led_red = false;
// static bool led_green = false;
// static bool led_blue = false;

// static uint8_t blink_counter;
// static struct etimer et;
static uint8_t color = 0;
static uint8_t blink_counter;

PROCESS(led_blink, "LED BLINK PATTERN");

PROCESS_THREAD(led_blink, ev, data){
	leds_off(LEDS_ALL);
	// color = (uint8_t)data;
	static struct etimer et;
	PROCESS_BEGIN();
	printf("Process Started\n");
	for(blink_counter = 0; blink_counter < N_BLINKS; blink_counter++){
		etimer_set(&et, BLINK_TON*CLOCK_SECOND);
		led_ctr(true,color);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		etimer_set(&et, BLINK_INTERVAL*CLOCK_SECOND);
		led_ctr(false,color);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	}
	leds_off(LEDS_ALL);

	color = 0;

	PROCESS_END();
}

void led_ctr(bool state,const uint8_t color){
	if(state){
		// printf("T_on, r:%i,g:%i,b:%i,\n",(color&0b100)==0b100,(color&0b010)==0b010,(color&0b001)==0b001);
		if((color&0b100)==0b100) leds_on(LEDS_RED);
		if((color&0b010)==0b010) leds_on(LEDS_GREEN);
		if((color&0b001)==0b001) leds_on(LEDS_BLUE);
	}
	else{
		// printf("T_off, r:%i,g:%i,b:%i,\n",(color&0b100)==0b100,(color&0b010)==0b010,(color&0b001)==0b001);
		if((color&0b100)==0b100) leds_off(LEDS_RED);
		if((color&0b010)==0b010) leds_off(LEDS_GREEN);
		if((color&0b001)==0b001) leds_off(LEDS_BLUE);
	}
}

void event_blink_leds(uint8_t color_){//0brgb
	color = color|color_;
	if(process_is_running(&led_blink)) blink_counter = 0;
	else process_start(&led_blink,"");
}