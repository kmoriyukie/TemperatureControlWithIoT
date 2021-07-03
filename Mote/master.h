#ifndef MASTER_H
#define MASTER_H

#define SEND_TO_CLOUD_INTERVAL 15
#define CLOUD_PACKAGE_SIZE 8
#define PACKAGE_BUFFER 16


#define MTRY 3
#define TIMOUT 5 //Seconds

#define DISC_TIMOUT 20 //Seconds

#include "contiki.h"
#include "list.h"
#include "stdbool.h"

#include "msg.h"

bool push_packet(struct slave_msg_t *packet);

bool pop_packet(struct slave_msg_t **packet);

void send_packets(void);

bool add_MOTE(uint8_t ID);

bool find_MOTE_localID(uint8_t ID, struct MOTE_t **item);

void exec_master_working(void);

void exit_master_working(void);

void exec_master_config(void);

void exit_master_config(void);

#endif
