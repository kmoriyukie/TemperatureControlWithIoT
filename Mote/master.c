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

#include "mqtt_states.h"

extern receive_mqtt_t RECEIVE_MODE;
extern send_mqtt_t SEND_MODE;
extern cloudmode_t CLOUD_MODE;

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

bool push_packet(struct slave_msg_t *packet){
	struct slave_msg_t *pck = NULL;
	struct MOTE_t *mote = NULL;
	if(find_MOTE_localID(packet->local_id, &mote)){
		if(!(mote->remote_id == packet->remote_id)) return false;
	}
	else return false;

	pck = malloc(sizeof(struct slave_msg_t));

	pck->local_id = packet->local_id;
	pck->remote_id = packet->remote_id;
	pck->temperature = packet->temperature;
	pck->humidity = packet->humidity;
	pck->airflow = packet->airflow;
	pck->battery = packet->battery;

	list_add(packet_list,pck);

	if(list_length(packet_list) >= CLOUD_PACKAGE_SIZE){
		//Send
	}

	return true;
}

bool pop_packet(struct slave_msg_t **packet){
	if(list_length(packet_list) <= 0) return false;

	*packet = list_pop(packet_list);

	return true;
}


/*--------------------------*/
/*----------Config----------*/
/*--------------------------*/

extern struct process border_router_process; //  /Connection/BorderRouter.c
extern struct process MQTTServerProcess; //  /Connection/mqttServer.c

static bool flag_mote_added = false;

PROCESS(master_config, "Master Config");

PROCESS_THREAD(master_config, ev, data){
	PROCESS_BEGIN();

	// Initialization

	RECEIVE_MODE = RECEIVE_NONE;
	SEND_MODE = SEND_NONE;

	list_init(motes_list);

	process_start(&border_router_process, "Border Router process");

	process_start(&MQTTServerProcess, "MQTT Server process");
	
	process_start(&coap_server_process, "CoAP process");

	// Configuration

	RECEIVE_MODE = RECEIVE_NONE;
	SEND_MODE = SEND_CONFIG_CLOUDMODE_REQUEST;


	while(CLOUD_MODE == STATUS_UNDEFINED){
		PROCESS_YIELD();
	}

	static struct etimer et_timeout;

	switch(CLOUD_MODE){
		case STATUS_UNDEFINED:
		break;
		case STATUS_CONFIG:
			etimer_set(&et_timeout, DISC_TIMOUT*CLOCK_SECOND);
			while(true){
				PROCESS_YIELD();
				if(flag_mote_added){
					flag_mote_added = false;
					etimer_reset(&et_timeout);
				}
				if((ev = etimer_expired(&et_timeout)) && (flag_mote_added == false)) break;
			}

			
		break;
		case STATUS_WORKING:

		break;
	}

	PROCESS_END();
}

bool add_MOTE(uint8_t ID){
	struct MOTE_t *mote = NULL;
	if(find_MOTE_localID(ID, &mote)) return false;
	else{
		// printf("Mote ADD\n");
		mote = malloc(sizeof(struct MOTE_t));
		mote->local_id = ID;
		mote->remote_id = 0;
		list_add(motes_list,mote);
		flag_mote_added = true;
		return true;
	}
}

bool find_MOTE_localID(uint8_t ID, struct MOTE_t **item){
	static struct MOTE_t *aux;
	for(aux = list_head(motes_list);aux != NULL; aux = list_item_next(aux)){
		if(aux->local_id == ID){
			*item = aux;
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
