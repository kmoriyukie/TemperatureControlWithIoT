#include "master.h"

#if CONTIKI_TARGET_ZOUL

#include "mqtt_states.h"

extern receive_mqtt_t RECEIVE_MODE;
extern send_mqtt_t SEND_MODE;
extern cloudmode_t CLOUD_MODE;

extern struct process coap_server_process; //  /Connection/CoAPServer.c

#include "dev/zoul-sensors.h"
#else /* Assumes the Z1 mote */
#include "sens_battery.h"
#endif

#include "msg.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"



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

#if CONTIKI_TARGET_ZOUL

extern struct process border_router_process; //  /Connection/BorderRouter.c
extern struct process MQTTServerProcess; //  /Connection/mqttServer.c

extern bool ids_sent;
extern bool ids_received;

#else
#endif

static uint8_t flag_mote_added = 0;


PROCESS(master_config, "");
PROCESS(config_cloudmode_request, "");
PROCESS(config_cloudmode_conf, "");
PROCESS(config_cloudmode_sendLocalIDS, "");
PROCESS(config_cloudmode_receiveRemoteIDS, "");
PROCESS(config_cloudmode_work, "");

// static process_event_t e_config_cloudmode_request, e_config_cloudmode_conf, e_config_cloudmode_sendLocalIDS;

PROCESS_THREAD(config_cloudmode_request, ev, data){
    PROCESS_BEGIN();

    #if CONTIKI_TARGET_ZOUL
    static int ret = 10;
    static int *dat = NULL;
	// PROCESS_PAUSE();

	// e_config_cloudmode_request = process_alloc_event();

    static struct etimer et_timeout;

    while(CLOUD_MODE == STATUS_UNDEFINED){
        PROCESS_YIELD();
        dat = data;
        if((ev == PROCESS_EVENT_MSG) && dat != NULL){
        	printf("CLOUDMODE Thread MSG\n");
        	if(*dat == 20){
        		printf("Start CLOUDMODE REQUEST\n");
        		SEND_MODE = SEND_CONFIG_CLOUDMODE_REQUEST;
        		RECEIVE_MODE = RECEIVE_CONFIG_CLOUDMODE_MODE;
        		etimer_set(&et_timeout, DISC_TIMOUT*CLOCK_SECOND);
        	}
        }
        if(etimer_expired(&et_timeout)) etimer_reset(&et_timeout);
    }

    SEND_MODE = SEND_NONE;
	RECEIVE_MODE = RECEIVE_NONE;

    printf("CLOUDMODE DONE\n");

    // process_poll(&master_config);

    process_post(&master_config, PROCESS_EVENT_CONTINUE, &ret);

    #else
    #endif

    PROCESS_END();
}

PROCESS_THREAD(config_cloudmode_conf, ev, data){
    PROCESS_BEGIN();

    #if CONTIKI_TARGET_ZOUL
    static int ret = 11;
    static int *dat = NULL;
	// PROCESS_PAUSE();

	// e_config_cloudmode_conf = process_alloc_event();

	static struct etimer et_timeout;

	static bool ev_started = false;

	while(true){
		PROCESS_YIELD();
		dat = data;
		if(dat != NULL){
			// printf("dat: %i\n", *dat);
        	if(*dat == 21){
        		printf("Start MOTE ADD\n");
        		SEND_MODE = SEND_NONE;
				RECEIVE_MODE = RECEIVE_NONE;
				ev_started = true;
        		etimer_set(&et_timeout, DISC_TIMOUT*CLOCK_SECOND);
        	}
        }
        if(ev_started && etimer_expired(&et_timeout)){
        	if(flag_mote_added){
        		flag_mote_added = false;
        		etimer_reset(&et_timeout);
        	}
        	else break;
        }
	}

	ev_started = false;

	SEND_MODE = SEND_NONE;
	RECEIVE_MODE = RECEIVE_NONE;

	printf("ADD MOTES DONE\n");


	process_post(&master_config, PROCESS_EVENT_CONTINUE, &ret);
    // process_poll(&master_config);

    #else
    #endif

    PROCESS_END();
}

PROCESS_THREAD(config_cloudmode_sendLocalIDS, ev, data){
    PROCESS_BEGIN();

    #if CONTIKI_TARGET_ZOUL
    extern uint8_t mote_count;
    static int ret = 12;
    static int *dat = NULL;

	// PROCESS_PAUSE();

	// e_config_cloudmode_sendLocalIDS = process_alloc_event();

	// printf("Send IDS thread\n");

	static struct etimer et_timeout;

	while(true){
		PROCESS_YIELD();
		dat = data;
		// printf("Send IDS\n");



		if((ev == PROCESS_EVENT_MSG) && dat != NULL){
        	if(*dat == 22){
        		printf("Start SEND LOCAL IDS\n");
        		SEND_MODE = SEND_CONFIG_IDS_REMLOC;
				RECEIVE_MODE = RECEIVE_CONFIG_IDS_REMLOC;
        		mote_count = 0;
        		etimer_set(&et_timeout, DISC_TIMOUT*CLOCK_SECOND);
        	}
        }
        if(etimer_expired(&et_timeout)){
        	if(!ids_sent) etimer_reset(&et_timeout);
			else break;
        }
	}

	SEND_MODE = SEND_NONE;
	RECEIVE_MODE = RECEIVE_NONE;

	printf("SEND IDS DONE\n");

	process_post(&master_config, PROCESS_EVENT_CONTINUE, &ret);
    // process_poll(&master_config);

    #else
    #endif

    PROCESS_END();
}

PROCESS_THREAD(config_cloudmode_receiveRemoteIDS, ev, data){
    PROCESS_BEGIN();

    #if CONTIKI_TARGET_ZOUL
    extern uint8_t mote_count;
    static int ret = 13;
    static int *dat = NULL;

	// PROCESS_PAUSE();

	// e_config_cloudmode_sendLocalIDS = process_alloc_event();

	// printf("Send IDS thread\n");

	static struct etimer et_timeout;

	while(true){
		PROCESS_YIELD();
		dat = data;
		// printf("Send IDS\n");



		if((ev == PROCESS_EVENT_MSG) && dat != NULL){
        	if(*dat == 23){
        		printf("Start RECEIVE REMOTE IDS\n");
        		mote_count = 0;
        		SEND_MODE = SEND_CONFIG_IDS_REMLOC;
				RECEIVE_MODE = RECEIVE_CONFIG_IDS_REMLOC;
        		etimer_set(&et_timeout, DISC_TIMOUT*CLOCK_SECOND);
        	}
        }
        if(etimer_expired(&et_timeout)){
        	if(!ids_received) etimer_reset(&et_timeout);
			else break;
        }
	}

	SEND_MODE = SEND_NONE;
	RECEIVE_MODE = RECEIVE_NONE;

	printf("RECEIVE IDS DONE\n");

	process_post(&master_config, PROCESS_EVENT_CONTINUE, &ret);
    // process_poll(&master_config);

    #else
    #endif

    PROCESS_END();
}

void set_ID_list(list_t *list);

PROCESS_THREAD(master_config, ev, data){
	PROCESS_BEGIN();

	PROCESS_PAUSE();

	#if CONTIKI_TARGET_ZOUL
	static int *ret;
	ret = malloc(sizeof(int));

	// Initialization

	RECEIVE_MODE = RECEIVE_NONE;
	SEND_MODE = SEND_NONE;

	list_init(motes_list);

	process_start(&config_cloudmode_request,&data);
	process_start(&config_cloudmode_conf,&data);
	process_start(&config_cloudmode_sendLocalIDS,&data);
	process_start(&config_cloudmode_receiveRemoteIDS,&data);

	process_start(&border_router_process, "Border Router process");

	process_start(&MQTTServerProcess, "MQTT Server process");
	
	process_start(&coap_server_process, "CoAP process");

	set_ID_list(&motes_list);

	PROCESS_PAUSE();

	
	int *dat = NULL;

	static bool configDONE = false;

	static struct etimer et;

	etimer_set(&et, 5 * CLOCK_SECOND);

	static bool starter = true;

	while(!configDONE){
		PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_CONTINUE || etimer_expired(&et));
		// PROCESS_WAIT_EVENT();

		if(etimer_expired(&et) && starter){
			starter = false;
			// Requesting CloudMode
			// printf("Starting, ret: %i\n",*ret);
			*ret = 20;
			// printf("Starting, ret: %i\n",*ret);
			process_post(&config_cloudmode_request, PROCESS_EVENT_MSG, ret);
			etimer_reset(&et);
		}

		if((ev == PROCESS_EVENT_CONTINUE) && (data != NULL)){
			dat = data;
			switch(*dat){
				case 10:
					// Return from -> Requesting CloudMode
					switch(CLOUD_MODE){
						case STATUS_UNDEFINED:
							printf("ERROR, UNDEFINED CLOUDMODE\n");
						break;
						case STATUS_CONFIG:
							// Requesting Add Motes
							printf("DAT == 10\n");
							*ret = 21;
							process_post(&config_cloudmode_conf, PROCESS_EVENT_MSG, ret);
						break;
						case STATUS_WORKING:
						break;
						}
				break;
				case 11:
					// Return from -> Add Motes

					// Sending Ids
					printf("DAT == 11\n");
					*ret = 22;
					process_post(&config_cloudmode_sendLocalIDS, PROCESS_EVENT_MSG, ret);
				break;
				case 12:
					*ret = 23;
					printf("DAT == 12\n");
					process_post(&config_cloudmode_receiveRemoteIDS, PROCESS_EVENT_MSG, ret);
				break;
				case 13:
					printf("DAT == 13\n");
					printf("Config DONE\n");
					configDONE = true;
				break;
			}
		}
		// if(etimer_expired(&et)) etimer_reset(&et);
	}


	#else
	#endif

	PROCESS_END();
}

PROCESS_THREAD(config_cloudmode_work, ev, data){
    PROCESS_BEGIN();

    PROCESS_END();
}

void send_ID_packet(char *buff){

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
		flag_mote_added = 1;
		printf("MOTE added\n");
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

bool update_MOTE_IDs(uint8_t local_ID, uint8_t remote_ID){
	/*static struct MOTE_t *aux;
	for(aux = list_head(motes_list);aux != NULL; aux = list_item_next(aux)){
		if(aux->local_id == local_ID){
			aux->remote_id = remote_ID;
			return true;
		}
	}*/
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
