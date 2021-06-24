#ifndef STATES_
#define STATES_

#include "stdint.h"
#include "stdbool.h"

typedef enum{
	MASTER = 1,
	SLAVE = 0
} ROLE_t;

typedef enum{
	CONFIG = 0,
	WORKING = 1
} MODE_t;

void initialize_states(void (*handle)(uint8_t));

void set_role(ROLE_t role_);

void set_mode(MODE_t mode_);

#endif
