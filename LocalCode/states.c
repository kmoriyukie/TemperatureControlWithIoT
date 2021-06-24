#include "states.h"

ROLE_t node_role = SLAVE;
MODE_t node_mode = CONFIG;

void (*blink_event_handle)(uint8_t);//0brgb

void initialize_states(void (*handle)(uint8_t)){
	blink_event_handle = handle;
}

void set_role(ROLE_t role_){
	node_role = role_;
	if(node_role) (*blink_event_handle)(0b010); //GREEN
	else (*blink_event_handle)(0b001); //BLUE
}

void set_mode(MODE_t mode_){
	node_mode = mode_;
	if(!node_mode) (*blink_event_handle)(0b100); //RED
}