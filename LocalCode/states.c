#include "states.h"

// #include "events_threads.h"

#include "stdio.h"

#include "master.h"
#include "slave.h"

ROLE_t node_role = SLAVE;
MODE_t node_mode = CONFIG;

task_running_t task_running = NONE;

void (*blink_event_handle)(void);//0brgb

void initialize_states(void (*handle)(void)){
	blink_event_handle = handle;
	static ROLE_t i_r = SLAVE;
	set_state(ROLE,&i_r);
}

void set_state(STATETYPE_t type,void *value){
	static ROLE_t *temp_role;
	static MODE_t *temp_mode;

	if(task_running != NONE){

		// exit_task = true;

		switch(task_running){
			case NONE:
			break;
			case MASTER_CONFIG:

			break;
			case MASTER_WORKING:

			break;
			case SLAVE_CONFIG:
				exit_slave_config();
			break;
			case SLAVE_WORKING:
				exit_slave_working();
			break;
		}
	}

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
					task_running = MASTER_CONFIG;
					state_exec_MASTER_CONFIG();
				break;
				case WORKING:
					task_running = MASTER_WORKING;
					state_exec_MASTER_WORKING();
				break;
			}
		break;
		case SLAVE:
			switch(node_mode){
				case CONFIG:
					task_running = SLAVE_CONFIG;
					exec_slave_config();
				break;
				case WORKING:
					task_running = SLAVE_WORKING;
					exec_slave_working();
				break;
			}
		break;
	}

	(*blink_event_handle)();
}





/****************************************************/
/***********************MASTER***********************/
/****************************************************/




// #include "master.h"

void state_exec_MASTER_CONFIG(){
	task_running = MASTER_CONFIG;
}

void state_exec_MASTER_WORKING(){
	task_running = MASTER_WORKING;
}




/****************************************************/
/************************SLAVE***********************/
/****************************************************/




// void state_exec_SLAVE_CONFIG(){
// 	task_running = SLAVE_CONFIG;
// }

// void state_exec_SLAVE_WORKING(){
// 	task_running = SLAVE_WORKING;
// 	static struct sensot_data *readings;
// 	event_readings(struct sensot_data *sens);
// }