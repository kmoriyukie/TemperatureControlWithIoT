#include "states.h"

#include "events_threads.h"

ROLE_t node_role = SLAVE;
MODE_t node_mode = CONFIG;

void (*blink_event_handle)(uint8_t);//0brgb

void initialize_states(void (*handle)(uint8_t)){
	blink_event_handle = handle;
	const ROLE_t i_r = MASTER;
	set_state(ROLE,&i_r);
}

void set_state(STATETYPE_t type,void *value){
	static ROLE_t *temp_role;
	static MODE_t *temp_mode;
	switch(type){
		case ROLE:
			temp_role = value;
			node_role = *temp_role;
			node_mode = CONFIG;
		break;
		case MODE:
			temp_mode = value;
			node_mode = *temp_mode;
		break;
	}

	switch(node_role){
		case MASTER:
			switch(node_mode){
				case CONFIG:
					state_exec_MASTER_CONFIG();
				break;
				case WORKING:
					state_exec_MASTER_WORKING();
				break;
			}
		break;
		case SLAVE:
			switch(node_mode){
				case CONFIG:
					state_exec_SLAVE_CONFIG();
				break;
				case WORKING:
					state_exec_SLAVE_WORKING();
				break;
			}
		break;
	}

	(*blink_event_handle)(getColor());
}

void state_exec_MASTER_CONFIG(){

}

void state_exec_MASTER_WORKING(){

}

void state_exec_SLAVE_CONFIG(){

}

void state_exec_SLAVE_WORKING(){

}