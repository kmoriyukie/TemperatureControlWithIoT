#ifndef SLAVE_H
#define SLAVE_H

#define N_READINGS 8
#define SR_READINGS 3

#define READINGS_TIME 5 //Seconds
#define MTRY 3
#define TIMOUT 5 //Seconds

void exec_slave_working(void);

void exit_slave_working(void);

void exec_slave_config(void);

void exit_slave_config(void);

#endif