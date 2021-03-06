#ifndef MASTER_H
#define MASTER_H

#define SEND_TO_CLOUD_INTERVAL 60
#define CLOUD_PACKAGE_SIZE 3
#define PACKAGE_BUFFER 16


#define MTRY 3
#define MASTER_TIMOUT 5 //Seconds

#define DISC_TIMOUT 15 //Seconds

#include "contiki.h"
#include "list.h"
#include "stdbool.h"

#include "msg.h"

typedef enum{
	SENS_IDLE,
	SENS_READY,
	SENS_DONE
} SENSOR_DATA_t;

// void send_packets_(void);

bool push_packet(struct slave_msg_t *packet);

bool pop_packet(struct slave_msg_t **packet);

void send_ID_packet(char *buff);

bool add_MOTE(uint8_t ID);

bool add_MOTE_REMLOC(uint8_t loc,uint8_t rem);

void printMoteList(void);

bool find_MOTE_localID(uint8_t ID, struct MOTE_t **item);

bool update_MOTE_IDs(uint8_t local_ID, uint8_t remote_ID);

void exec_master_working(void);

void exit_master_working(void);

void exec_master_config(void);

void exit_master_config(void);

bool find_onBlink_list(uint8_t ID, struct blink_t **item);

void blinkList_push(uint8_t loc_id);

void blinkList_remove(struct blink_t **item);

#endif
