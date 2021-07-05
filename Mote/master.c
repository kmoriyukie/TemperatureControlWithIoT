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

#else
#endif

static uint8_t flag_mote_added = 0;


PROCESS(master_config, "");
PROCESS(config_cloudmode_request, "");
PROCESS(config_cloudmode_conf, "");
PROCESS(config_cloudmode_sendLocalIDS, "");
PROCESS(config_cloudmode_work, "");

// static process_event_t e_config_cloudmode_request, e_config_cloudmode_conf, e_config_cloudmode_sendLocalIDS;

PROCESS_THREAD(config_cloudmode_request, ev, data){
    PROCESS_BEGIN();

    #if CONTIKI_TARGET_ZOUL
    static int ret = 0;
    static int *dat = NULL;
	// PROCESS_PAUSE();

	// e_config_cloudmode_request = process_alloc_event();

    static struct etimer et_timeout;

    while(CLOUD_MODE == STATUS_UNDEFINED){
    	SEND_MODE = SEND_CONFIG_CLOUDMODE_REQUEST;
        PROCESS_YIELD();
        dat = data;
        if(dat != NULL){
        	if(*dat == 10) etimer_set(&et_timeout, DISC_TIMOUT*CLOCK_SECOND);
        }
        if(etimer_expired(&et_timeout)) etimer_reset(&et_timeout);
    }

    SEND_MODE = SEND_NONE;

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
    static int ret = 1;
    static int *dat = NULL;
	// PROCESS_PAUSE();

	// e_config_cloudmode_conf = process_alloc_event();

	static struct etimer et_timeout;

	while(true){
		PROCESS_YIELD();
		dat = data;
		if(dat != NULL){
        	if(*dat == 11) etimer_set(&et_timeout, DISC_TIMOUT*CLOCK_SECOND);
        }
        if(etimer_expired(&et_timeout)){
        	if(flag_mote_added){
        		flag_mote_added = false;
        		etimer_reset(&et_timeout);
        	}
        	else break;
        }
	}

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
    static int ret = 2;
    static int *dat = NULL;

	// PROCESS_PAUSE();

	// e_config_cloudmode_sendLocalIDS = process_alloc_event();

	// printf("Send IDS thread\n");

	static struct etimer et_timeout;

	while(true){
		SEND_MODE = SEND_CONFIG_IDS_REMLOC;
		PROCESS_YIELD();
		dat = data;
		// printf("Send IDS\n");
		if(dat != NULL){
        	if(*dat == 12) etimer_set(&et_timeout, DISC_TIMOUT*CLOCK_SECOND);
        }
        if(etimer_expired(&et_timeout)){
        	if(!ids_sent) etimer_reset(&et_timeout);
			else break;
        }
	}

	SEND_MODE = SEND_NONE;

	printf("SEND IDS DONE\n");

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

	process_start(&border_router_process, "Border Router process");

	process_start(&MQTTServerProcess, "MQTT Server process");
	
	process_start(&coap_server_process, "CoAP process");

	set_ID_list(&motes_list);

	PROCESS_PAUSE();

	// Requesting CloudMode
	*ret = 10;
	process_post(&config_cloudmode_request, PROCESS_EVENT_MSG, ret);

	while(true){
		// PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT);
		PROCESS_WAIT_EVENT();

		if(data != NULL){
			int *dat = data;
			if(*dat == 0){
				// Return from -> Requesting CloudMode

				switch(CLOUD_MODE){
					case STATUS_UNDEFINED:
						printf("ERROR, UNDEFINED CLOUDMODE\n");
					break;
					case STATUS_CONFIG:
						// Requesting Add Motes
						*ret = 11;
						process_post(&config_cloudmode_conf, PROCESS_EVENT_MSG, ret);
					break;
					case STATUS_WORKING:
					break;
				}
			}
			if(*dat == 1){
				// Return from -> Add Motes

				// Sending Ids
				*ret = 12;
				process_post(&config_cloudmode_sendLocalIDS, PROCESS_EVENT_MSG, ret);
			}
			if(*dat == 2){
				break;
			}

		/*switch(ev){
			case e_config_cloudmode_request:

			break;
			case e_config_cloudmode_conf:
			break;
			case config_cloudmode_sendLocalIDS:
			break;
		}*/
		}
	}

	// process_start(&config_cloudmode_request,&data);
	// PROCESS_PAUSE();

	/*
	process_poll(&config_cloudmode_request);
	PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

	switch(CLOUD_MODE){
		case STATUS_UNDEFINED:
		break;
		case STATUS_CONFIG:

			process_start(&config_cloudmode_conf,&data);
			// PROCESS_PAUSE();
			process_poll(&config_cloudmode_conf);
			PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

			// Sending Local Ids
			// int a = 5;

			// process_start(&config_cloudmode_sendLocalIDS,&data);
			// // PROCESS_PAUSE();
			// process_poll(&config_cloudmode_sendLocalIDS);
			// PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

		break;
		case STATUS_WORKING:
			printf("Exec Working\n");

		break;
	}*/



	// Adding Motes

	




	printf("\n\n\nOUT BLOCK\n\n\n");
	/*
	switch(CLOUD_MODE){
		case STATUS_UNDEFINED:
		break;
		case STATUS_CONFIG:
			printf("Exec Config\n");
			etimer_set(&et_timeout2, DISC_TIMOUT*CLOCK_SECOND);
			// etimer_reset(&et_timeout);
			while(1){
				printf("\n\n\nyield__\n\n\n");
				// PROCESS_YIELD_UNTIL();
				PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et_timeout2) == 1);
				printf("\n\n\nyield2__\n\n\n");

				if(flag_mote_added == 1){
					printf("\n\n\nadded2__\n\n\n");
					flag_mote_added = 0;
					etimer_reset(&et_timeout2);
				}
				else break;

				// if(etimer_expired(&et_timeout)){
				// 	printf("\n\n\nexpired__\n\n\n");
				// 	if(flag_mote_added == false) break;
				// 	else etimer_reset(&et_timeout);
				// }
				// if(flag_mote_added){
				// 	printf("\n\n\nmote add__\n\n\n");
				// 	flag_mote_added = false;
				// 	etimer_reset(&et_timeout);
				// }
				// etimer_reset(&et_timeout);
			}
			
			printf("Sending IDS to cloud\n");
			SEND_MODE = SEND_CONFIG_IDS_REMLOC;
			// send_motes();

		break;
		case STATUS_WORKING:
			printf("Exec Working\n");

		break;
	}*/


	/*
	static uint8_t state = 0;

	static struct etimer et_timeout;

    etimer_set(&et_timeout, DISC_TIMOUT*CLOCK_SECOND);

    while(true){
    	PROCESS_YIELD();
    	switch(state){
			case 0:
				RECEIVE_MODE = RECEIVE_NONE;
				SEND_MODE = SEND_CONFIG_CLOUDMODE_REQUEST;
				state = 1;
				etimer_reset(&et_timeout);
			break;
			case 1:
				if(CLOUD_MODE == STATUS_UNDEFINED){
					if(etimer_expired(&et_timeout) etimer_reset(&et_timeout);	
				}
				else{
					switch(CLOUD_MODE){
						case STATUS_UNDEFINED:
							printf("Undefined Cloud Mode\n");
							state = 0;
						break;
						case STATUS_CONFIG:
							printf("Config Cloud Mode\n");
							state = 10;
						break;
						case STATUS_WORKING:
							printf("Working Cloud Mode\n");
							state = 20;
						break;
					}
					RECEIVE_MODE = RECEIVE_NONE;
					SEND_MODE = SEND_NONE;
					// state++;
					etimer_reset(&et_timeout);
				}
			break;
			case 10:
				if(etimer_expired(&et_timeout)){
					if(flag_mote_added == 1){
						flag_mote_added = 0;
						etimer_reset(&et_timeout);	
					}
				}
				if(flag_mote_added == 1){
					flag_mote_added = 0;
					etimer_reset(&et_timeout2);
				}
			break;
				while(1){
					printf("\n\n\nyield__\n\n\n");
					// PROCESS_YIELD_UNTIL();
					PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et_timeout2) == 1);
					printf("\n\n\nyield2__\n\n\n");

					if(flag_mote_added == 1){
						printf("\n\n\nadded2__\n\n\n");
						flag_mote_added = 0;
						etimer_reset(&et_timeout2);
					}
					else break;

					if(etimer_expired(&et_timeout)){
						printf("\n\n\nexpired__\n\n\n");
						if(flag_mote_added == false) break;
						else etimer_reset(&et_timeout);
					}
					if(flag_mote_added){
						printf("\n\n\nmote add__\n\n\n");
						flag_mote_added = false;
						etimer_reset(&et_timeout);
					}
					etimer_reset(&et_timeout);
				}
				
				printf("Sending IDS to cloud\n");
				SEND_MODE = SEND_CONFIG_IDS_REMLOC;
				// send_motes();
		}
    }*/

    /*
	RECEIVE_MODE = RECEIVE_NONE;
	SEND_MODE = SEND_CONFIG_CLOUDMODE_REQUEST;

	static struct etimer et_timeout;
	static struct etimer et_timeout2;

	etimer_set(&et_timeout, DISC_TIMOUT*CLOCK_SECOND);

	uint8_t state = 0;*/

	// switch(state){
	// 	case 0:

	// 	break;
	// }

	
	// while(CLOUD_MODE == STATUS_UNDEFINED){
	// 	PROCESS_YIELD();
	// 	if(etimer_expired(&et_timeout)){
	// 		// printf("\n\n\nP_cloudmode\n\n\n");
	// 		etimer_reset(&et_timeout);
	// 	}
	// }

	// SEND_MODE = SEND_NONE;

	printf("\n\n\nOUT WHILE\n\n\n");

	printf("\n\n\nB switch\n  CLOUDMODE: %u'n  SEND_MODE: %u\n\n\n",CLOUD_MODE,SEND_MODE);

	/*
	switch(CLOUD_MODE){
		case STATUS_UNDEFINED:
		break;
		case STATUS_CONFIG:
			printf("Exec Config\n");
			etimer_set(&et_timeout2, DISC_TIMOUT*CLOCK_SECOND);
			// etimer_reset(&et_timeout);
			while(1){
				printf("\n\n\nyield__\n\n\n");
				// PROCESS_YIELD_UNTIL();
				PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et_timeout2) == 1);
				printf("\n\n\nyield2__\n\n\n");

				if(flag_mote_added == 1){
					printf("\n\n\nadded2__\n\n\n");
					flag_mote_added = 0;
					etimer_reset(&et_timeout2);
				}
				else break;

				// if(etimer_expired(&et_timeout)){
				// 	printf("\n\n\nexpired__\n\n\n");
				// 	if(flag_mote_added == false) break;
				// 	else etimer_reset(&et_timeout);
				// }
				// if(flag_mote_added){
				// 	printf("\n\n\nmote add__\n\n\n");
				// 	flag_mote_added = false;
				// 	etimer_reset(&et_timeout);
				// }
				// etimer_reset(&et_timeout);
			}
			
			printf("Sending IDS to cloud\n");
			SEND_MODE = SEND_CONFIG_IDS_REMLOC;
			// send_motes();

		break;
		case STATUS_WORKING:
			printf("Exec Working\n");

		break;
	}*/

	printf("Out switch\n");

	#else
	#endif

	PROCESS_END();
}

PROCESS_THREAD(config_cloudmode_work, ev, data){
    PROCESS_BEGIN();

    PROCESS_END();
}

void send_motes(void){
	/*
	static uint8_t i;
	static uint8_t j;

	static char buff[70];

	static uint8_t ids_buff[5];

	static struct MOTE_t *aux;

	aux = list_head(motes_list);

	for(j = 0; j < list_length(motes_list); j+=5){

		for(i = 0; i < 5; i++){
			if(aux == NULL){
				ids_buff[i] = -1;
				continue;
			}
			else{
				ids_buff[i] = aux->local_id;
				aux = list_item_next(aux);
			}

		}
		sprintf(buff,"{\"local_IDs\": {\"l%u\": %u,
									   \"l%u\": %u,
									   \"l%u\": %u,
									   \"l%u\": %u,
									   \"l%u\": %u}}\n",
									 j+1,ids_buff[0],
									 j+2,ids_buff[1],
									 j+3,ids_buff[2],
									 j+4,ids_buff[3],j+5,ids_buff[4]);

		send_ID_packet(buff);
	}*/
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
