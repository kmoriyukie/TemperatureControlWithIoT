// #include ""

#ifndef RES_CONFIG_MQTT_C_
#define RES_CONFIG_MQTT_C_

#include "mqtt_states.h"

#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"

#include "mqttCOM.h"
// #include "mqttMacros.h"

extern receive_mqtt_t RECEIVE_MODE;
extern send_mqtt_t SEND_MODE;
extern cloudmode_t CLOUD_MODE;

bool update_MOTE_IDs(uint8_t local_ID, uint8_t remote_ID);

void readJSON_i(const char *json, int *params_u);

bool ids_sent = false;

// bool msg_sent = false;

uint8_t mote_count = 0;
bool ids_received = false;

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

	int params_u[2];
	readJSON_i(msg, params_u);

	switch(params_u[1]){
		case 0:
			CLOUD_MODE = STATUS_CONFIG;
			printf("\n\nCLOUD Config Mode\n\n\n");
		break;
		case 1:
			CLOUD_MODE = STATUS_WORKING;
			printf("\n\nCLOUD Working Mode\n\n\n");
		break;
	}



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

void receive_ids(const char *msg,uint16_t len);

void send_local_ids(void);

void set_ID_list(list_t *list){
	aux_motes_list = list;
}


void receive_ids(const char *msg,uint16_t len){
	// receive ids_confirmation

	// For {
	// 		Receive Local/Remote ids
	// }
	//
	// Receive end of ids - N_ids



	// remote_count

	ids_received = false;


	static uint8_t i;
	static int param_i[8];
	readJSON_i(msg,param_i);


	for(i = 0; i < 3; i++){
		// printf("%i\n", param_u[i]);
		if(param_i[2*i+1] != -1) update_MOTE_IDs(param_i[2*i+1],param_i[2*i+2]);
		printf("L: %i, R: %i\n",param_i[2*i+1],param_i[2*i+2]);
	}

	if(param_i[7] != -1) ids_received = true;
	else ids_received = false;
}

void send_local_ids(void){
	// For {
	// 		Send local ids
	// }
	//
	// Send end of ids - N_ids

	ids_sent = false;
	
	static uint8_t i;
	// static uint8_t j;

	static char buff[80];

	static int ids_buff[5];

	static struct MOTE_t *aux;

	aux = list_head(*aux_motes_list);

	if(mote_count < list_length(*aux_motes_list)){
		// msg_sent = false;
		for(i = 0; i < mote_count; i++) aux = list_item_next(aux);
		for(i = mote_count; i < mote_count+5; i++){
			if(aux == NULL){
				ids_buff[i] = -1;
				continue;
			}
			else{
				ids_buff[i] = aux->local_id;
				aux = list_item_next(aux);
			}

		}

		if(mote_count+5>=list_length(*aux_motes_list)){
			sprintf(buff,"{\"local_IDs\": {\"l%u\": %i,"
									  "\"l%u\": %i,"
									  "\"l%u\": %i,"
									  "\"l%u\": %i,"
									  "\"l%u\": %i},\"N\": %u}",
									 mote_count+1,ids_buff[0],
									 mote_count+2,ids_buff[1],
									 mote_count+3,ids_buff[2],
									 mote_count+4,ids_buff[3],
									 mote_count+5,ids_buff[4],list_length(*aux_motes_list));
		}
		else{
			sprintf(buff,"{\"local_IDs\": {\"l%u\": %i,"
									  "\"l%u\": %i,"
									  "\"l%u\": %i,"
									  "\"l%u\": %i,"
									  "\"l%u\": %i},\"N\": %u}",
									 mote_count+1,ids_buff[0],
									 mote_count+2,ids_buff[1],
									 mote_count+3,ids_buff[2],
									 mote_count+4,ids_buff[3],
									 mote_count+5,ids_buff[4],list_length(*aux_motes_list));
		}


		mqttcom_pub(CONFIG_MOTE_ID_TOPIC,buff);
		mote_count+=5;
	}
	else ids_sent = true;

	// for(j = 0; j < list_length(*aux_motes_list); j+=5){
	// }

	// Confirmation







	// Send ids_confirmation
}

void request_remote_ids(void){
	printf("Sending Remote Ids Request\n");
	static char REMOTE_REQ[] = "{\"device\": \"RE-Mote\"}";
	mqttcom_pub(CONFIG_MOTE_REMOT_ID_TOPIC,REMOTE_REQ);
}




void receive_blink_request(const char *msg,uint16_t len){

	int params_u[2];
	readJSON_i(msg, params_u);

	printf("Blink Request MQTT %i,%u\n",params_u[0],params_u[1]);

	if(params_u[1] >= 0 && params_u[1] <= 255) blinkList_push((uint8_t)params_u[1]);
}

#endif