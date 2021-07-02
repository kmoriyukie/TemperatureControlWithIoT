#include "master.h"

#if CONTIKI_TARGET_ZOUL
#include "dev/zoul-sensors.h"
#else /* Assumes the Z1 mote */
#include "sens_battery.h"
#endif

#include "msg.h"
// #include "stdbool.h"
#include "stdlib.h"

#include "stdio.h"

extern struct process coap_server_process; //  /Connection/CoAPServer.c

/*---------------------------*/
/*----------Working----------*/
/*---------------------------*/

LIST(motes_list);

LIST(packet_list);

struct slave_msg_t *packet;

void send_packets(void){


	static uint8_t n = 0;
	n = list_length(packet_list); 
	static uint8_t i;
	for(i = 0; i < n; i++){
		// list_pop(packet_list);	
	}
}

PROCESS(master_working, "Master Working");

PROCESS_THREAD(master_working, ev, data){
	PROCESS_BEGIN();

	list_init(packet_list);

	// list_add(packet_list,*packet);
	// list_pop(packet_list);
	// list_length(packet_list);

	static struct etimer et;

	etimer_set(&et, SEND_TO_CLOUD_INTERVAL*CLOCK_SECOND);

	while(true){

		PROCESS_YIELD();

		if(ev == etimer_expired(&et) || (list_length(packet_list) >= CLOUD_PACKAGE_SIZE)){
			send_packets();
		}

		// PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));



		etimer_reset(&et);
	}

	PROCESS_END();
}




/*--------------------------*/
/*----------Config----------*/
/*--------------------------*/

PROCESS(master_config, "Master Config");

PROCESS_THREAD(master_config, ev, data){
	PROCESS_BEGIN();

	list_init(motes_list);

	process_start(&coap_server_process, "CoAP process");




	PROCESS_END();
}

bool add_MOTE(uint8_t ID){
	struct MOTE_t *mote = NULL;
	if(find_MOTE_localID(ID, mote)) return false;
	else{
		// printf("Mote ADD\n");
		mote = malloc(sizeof(struct MOTE_t));
		mote->local_id = ID;
		mote->remote_id = 0;
		list_add(motes_list,mote);
		return true;
	}
}

bool find_MOTE_localID(uint8_t ID, struct MOTE_t **item){
	// item = list_head(motes_list);
	// while(item != NULL){
	// 	if(item->local_id == ID) return true;
	// 	item = list_item_next(item);
	// }
	static struct MOTE_t *aux;
	for(aux = list_head(motes_list);aux != NULL; aux = list_item_next(aux)){
		if(aux->local_id == ID){
			*item = aux;
			printf("Remote_ID: %u\n", aux->remote_id);
			return true;
		}
	}
	item = NULL;
	return false;
}

void exec_master_working(void){
	process_start(&master_working,"");
}

void exit_master_working(void){
	process_exit(&master_working);
}

void exec_master_config(void){
	process_start(&master_config,"");
}

void exit_master_config(void){
	process_exit(&master_config);
}