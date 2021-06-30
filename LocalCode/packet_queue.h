#ifndef PACKET_QUEUE_H_
#define PACKET_QUEUE_H_

#include "msg.h"

#define PACKAGE_BUFFER 16

struct packet_queue{
	uint8_t head;
	uint8_t tail;

	uint8_t size;

	struct slave_msg_t data[PACKAGE_BUFFER];


};

void initialize(struct packet_queue *pk);

void push(struct slave_msg_t data);

struct slave_msg_t pop(void);

#endif