// #include ""

#ifndef RES_CONFIG_MQTT_C_
#define RES_CONFIG_MQTT_C_

#include "mqtt_states.h"

#include "stdlib.h"
#include "stdint.h"

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
			printf("CLOUD Config Mode\n");
		break;
		case 1:
			CLOUD_MODE = STATUS_WORKING;
			printf("CLOUD Working Mode\n");
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






void receive_ids(void);

void send_ids(void);

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

	// Send ids_confirmation
}

#endif