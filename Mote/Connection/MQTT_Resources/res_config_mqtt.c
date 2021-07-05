// #include ""

#ifndef RES_CONFIG_MQTT_C_
#define RES_CONFIG_MQTT_C_

#include "mqtt_states.h"

#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

#include "mqttCOM.h"
// #include "mqttMacros.h"

extern receive_mqtt_t RECEIVE_MODE;
extern send_mqtt_t SEND_MODE;
extern cloudmode_t CLOUD_MODE;


// extern struct mqtt_connection conn;
// extern char *pub_topic;
// extern char *sub_topic;
// extern char app_buffer[APP_BUFFER_SIZE];

void receive_cloudmode(const char *msg,uint16_t len);

void send_cloudmode(void);

void receive_cloudmode(const char *msg,uint16_t len){
	// Receive cloudmode -> CONFIG_CLOUDMODE_MODE
	// printf("MSG: %s\nSize: %u\n", msg,len);

	// Change CLOUD_MODE

	uint8_t params_u[1];
	readJSON_uf(msg, params_u,NULL);

	switch(params_u[0]){
		case 0:
			CLOUD_MODE = STATUS_CONFIG;
			printf("\n\nCLOUD Config Mode\n\n\n");
		break;
		case 1:
			CLOUD_MODE = STATUS_WORKING;
			printf("\n\nCLOUD Working Mode\n\n\n");
		break;
	}

	// if(params_u[0] == 0){
	// 	CLOUD_MODE = STATUS_CONFIG;
	// 	printf("CLOUD Config Mode\n");
	// }
	// else{
	// 	if(){
	// 	CLOUD_MODE = STATUS_WORKING;
	// 	printf("CLOUD Working Mode\n");
	// }





	// Receive config_mode -> CONFIG_CLOUDMODE_SETCONFIG
}

void send_cloudmode(void){
	// Send Request -> CLOUDMODE_REQUEST
	printf("Sending CloudMode Request\n");
	static char CLOUDMODE_REQ[] = "{\"device\": \"RE-Mote\"}";
	mqttcom_pub(CONFIG_MOTE_CLOUDMODE_TOPIC,CLOUDMODE_REQ);
}


#include "list.h"
#include "msg.h"

void set_ID_list(list_t *list);

static list_t *aux_motes_list;

void receive_ids(void);

void send_ids(void);

void set_ID_list(list_t *list){
	aux_motes_list = list;
}

void receive_ids(void){
	// receive ids_confirmation

	// For {
	// 		Receive Local/Remote ids
	// }
	//
	// Receive end of ids - N_ids
}

void send_ids(void){
	// For {
	// 		Send local ids
	// }
	//
	// Send end of ids - N_ids


	
	static uint8_t i;
	static uint8_t j;

	static char buff[70];

	static uint8_t ids_buff[5];

	static struct MOTE_t *aux;

	aux = list_head(*aux_motes_list);

	for(j = 0; j < list_length(*aux_motes_list); j+=5){

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

		sprintf(buff,"{\"local_IDs\": {\"l%u\": %u,"
									  "\"l%u\": %u,"
									  "\"l%u\": %u,"
									  "\"l%u\": %u,"
									  "\"l%u\": %u}}\n",
									 j+1,ids_buff[0],
									 j+2,ids_buff[1],
									 j+3,ids_buff[2],
									 j+4,ids_buff[3],
									 j+5,ids_buff[4]);

		mqttcom_pub(CONFIG_MOTE_ID_TOPIC,buff);
	}









	// Send ids_confirmation
}

#endif