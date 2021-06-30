#ifndef STATES_
#define STATES_

#include "stdint.h"
#include "stdbool.h"

#include "msg.h"

typedef enum{
	MODE = 0,
	ROLE = 1
} STATETYPE_t;

typedef enum{
	MASTER = 1,
	SLAVE = 0
} ROLE_t;

typedef enum{
	CONFIG = 0,
	WORKING = 1
} MODE_t;

typedef enum{
	NONE,
	MASTER_CONFIG,
	MASTER_WORKING,
	SLAVE_CONFIG,
	SLAVE_WORKING
} task_running_t;

// bool exit_flag = false;

void initialize_states(void (*handle)(void));

// void set_role(ROLE_t role_);

// void set_mode(MODE_t mode_);

void set_state(STATETYPE_t type,void *value);

void state_exec_MASTER_CONFIG();

void state_exec_MASTER_WORKING();

// void state_exec_SLAVE_CONFIG();

// void state_exec_SLAVE_WORKING();

#endif
