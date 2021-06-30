#include "packet_queue.h"

struct packet_queue *pk;

void initialize(struct packet_queue *packet){
	(*packet).head = 0;
	(*packet).tail = 0;

	(*packet).size = 0;

	pk = packet;
}

void push(struct slave_msg_t data){
	if((*pk).size != 0){
		(*pk).tail++;
	}
	(*pk).data[(*pk).tail] = data;
	(*pk).size++;
}

struct slave_msg_t pop(void){
	if((*pk).size != 1){
		(*pk).tail--;
	}
}