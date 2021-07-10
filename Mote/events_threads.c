#include "events_threads.h"

#include "contiki.h"
#include "stdio.h"
#include "stdlib.h"

#include "sys/etimer.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "lib/sensors.h"

#include "states.h"

/****************************************************/
/************************LED*************************/
/****************************************************/

// uint8_t led_state = 0b0;

extern ROLE_t node_role;
extern MODE_t node_mode;

extern uint8_t EVblinkLED;

// static uint8_t color = 0;

static uint8_t blink_counter;	

PROCESS(led_blink, "");

PROCESS(my_blink, "");
void led_ctr(const uint8_t color);

PROCESS_THREAD(led_blink, ev, data){
	PROCESS_BEGIN();
	leds_off(LEDS_ALL);


	static struct etimer et;
	static int *dat = NULL;

	static uint8_t f_color;


	static uint8_t b_color = 0;	

	static uint8_t myd;

	bool blinking = 0;

	etimer_set(&et, BLINK_TON*CLOCK_SECOND);
	while(1){
		PROCESS_YIELD();
		dat = data;
		if((ev == PROCESS_EVENT_MSG) && dat != NULL){
			switch(*dat){ 
				case 71: 
					printf("REQ 71\n");
					blink_counter = N_BLINKS;
					f_color = (node_mode==CONFIG)<<2;
				break;
				case 72: 
					printf("REQ 72\n");
					f_color = (node_mode==CONFIG)<<2;
				break;
			}
		}

		if(etimer_expired(&et) && blink_counter > 0){
			if(!blinking) process_start(&my_blink, &f_color);
			myd = *((uint8_t *)data);
			while(ev != PROCESS_EVENT_MSG && myd != 1){
				PROCESS_YIELD();
			}
			blinking = 0;
			etimer_reset(&et);
		}
	}
	PROCESS_END();
}


PROCESS_THREAD(my_blink, ev, data){
	PROCESS_BEGIN();

	static struct etimer et2;
	etimer_set(&et2,BLINK_INTERVAL*CLOCK_SECOND);
	
	static uint8_t f_color;
	f_color = *((uint8_t *)data);
	static uint8_t actColor = 0;
	
	if(blink_counter > 0){
		actColor = ((EVblinkLED|f_color))|getColor();
		printf("actColor = %u\n", actColor);

		blink_counter--;
	}
	else{
		actColor = ((EVblinkLED|f_color));	
		printf("actColor = %u\n", actColor);

	}
	led_ctr(actColor);
	
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et2));
	actColor = (((~EVblinkLED&f_color)<<2)|0b00);
	led_ctr(actColor);

	leds_off(LEDS_RED);
	
	static uint8_t ok = 1;
	process_post(&led_blink, PROCESS_EVENT_MSG, &ok);
	PROCESS_END();
}




void led_ctr(const uint8_t color){
	if((color&0b100)==0b100) leds_on(LEDS_RED);
	else leds_off(LEDS_RED);
	if((color&0b010)==0b010) leds_on(LEDS_GREEN);
	else leds_off(LEDS_GREEN);
	if((color&0b001)==0b001) leds_on(LEDS_BLUE);
	else leds_off(LEDS_BLUE);
}

void event_blink_leds(uint8_t *ret){//0brgb
	if(!process_is_running(&led_blink)) process_start(&led_blink,"");
	else{
		process_post(&led_blink, PROCESS_EVENT_MSG, ret);
	}
}

// extern ROLE_t node_role;
// extern MODE_t node_mode;

uint8_t getColor(void){
	return ( ((node_role==1)<<1)|(node_role==0) );
}


/****************************************************/
/***********************Button***********************/
/****************************************************/

PROCESS(button_pressed, "Button Pressed");

PROCESS_THREAD(button_pressed, ev, data){
	PROCESS_BEGIN();

	static uint8_t button_count = 0;
	static bool button_debounce = false;
	static struct etimer et;
	static struct etimer debouncer;

	SENSORS_ACTIVATE(button_sensor);

	printf("Button Event Started\n");



	static MODE_t mod;
	static ROLE_t rol;

	static uint8_t *ret;
	ret = malloc(sizeof(uint8_t));



	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&debouncer) || etimer_expired(&et) || ((data == &button_sensor) && !button_debounce));
		if(etimer_expired(&debouncer) && button_debounce){
				#if CONTIKI_TARGET_ZOUL
				if(button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) == BUTTON_SENSOR_PRESSED_LEVEL){
					button_debounce = true;
					etimer_reset(&debouncer);
				}
				else{
					button_debounce = false;
				}
				#else
				button_debounce = false;
				#endif
		}
		else{
			if(data == &button_sensor){
				etimer_set(&et, 1*CLOCK_SECOND);
				etimer_set(&debouncer, DEBOUNCE_TIME*CLOCK_SECOND);
				button_debounce = true;
				button_count++;
			}
			else{
				switch(button_count){
					case 1:
						*ret = 71;
						event_blink_leds(ret);
						// printf("1 click\n");
					break;
					case 2:
						mod = !node_mode;
						set_state(MODE,&mod);
						// printf("2 click\n");
					break;
					case 3:
						rol = !node_role;
						set_state(ROLE,&rol);
						// printf("3 click\n");
					break;
				}

				printf("Count: %u", button_count);
				button_count = 0;
			}
		}
	}

	PROCESS_END();
}