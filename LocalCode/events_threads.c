#include "events_threads.h"

#include "contiki.h"
#include "stdio.h"

#include "sys/etimer.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "lib/sensors.h"

#include "states.h"

/****************************************************/
/************************LED*************************/
/****************************************************/


static uint8_t color = 0;
static uint8_t blink_counter;

PROCESS(led_blink, "LED BLINK PATTERN");

PROCESS_THREAD(led_blink, ev, data){
	PROCESS_BEGIN();
	leds_off(LEDS_ALL);
	static struct etimer et;

	printf("Led Event Started\n");
	for(blink_counter = 0; blink_counter < N_BLINKS; blink_counter++){
		etimer_set(&et, BLINK_TON*CLOCK_SECOND);
		led_ctr(color);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		etimer_set(&et, BLINK_INTERVAL*CLOCK_SECOND);
		leds_off(LEDS_ALL);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	}
	leds_off(LEDS_ALL);

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

void event_blink_leds(uint8_t color_){//0brgb
	color = color_;
	if(process_is_running(&led_blink)) blink_counter = 0;
	else process_start(&led_blink,"");
}

extern ROLE_t node_role;
extern MODE_t node_mode;

uint8_t getColor(void){
	return ( ((node_mode==0)<<2)|((node_role==1)<<1)|(node_role==0) );
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
						event_blink_leds(getColor());
					break;
					case 2:
						mod = !node_mode;
						set_state(MODE,&mod);
					break;
					case 3:
						rol = !node_role;
						set_state(ROLE,&rol);
					break;
				}
				button_count = 0;
			}
		}
	}

	PROCESS_END();
}