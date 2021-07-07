#ifndef RES_CONFIG_MQTT_C_
#define RES_CONFIG_MQTT_C_

#include "mqtt_states.h"

#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"

#include "mqttCOM.h"

extern receive_mqtt_t RECEIVE_MODE;
extern send_mqtt_t SEND_MODE;

bool send_sensors_packet(void);

void readJSON_i(const char *json, int *params_u);

bool send_sensors_packet(void){
	printf("CLOUD SENS: %s\n", cloud_sens);
	mqttcom_pub(SENSOR_MOTE,cloud_sens);
}

#endif
