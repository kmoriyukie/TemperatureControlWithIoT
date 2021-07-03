// #include ""

#ifndef RES_CONFIG_MQTT_C_
#define RES_CONFIG_MQTT_C_

void receive_cloudmode(void);

void send_cloudmode(void);

void receive_cloudmode(void){
	// Receive cloudmode -> CONFIG_CLOUDMODE_MODE

	// Receive config_mode -> CONFIG_CLOUDMODE_SETCONFIG
}

void send_cloudmode(void){
	// Send Request -> CLOUDMODE_REQUEST
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