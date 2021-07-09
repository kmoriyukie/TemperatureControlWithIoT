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

#include "states.h"

uint8_t MASTER_BATTERY;


/*---------------------------*/
/*----------Working----------*/
/*---------------------------*/

LIST(motes_list);

LIST(packet_list);

static uint8_t m_remote_ID = 0;

SENSOR_DATA_t sens_status = SENS_IDLE;

struct slave_msg_t *packet;

PROCESS(master_working, "Master Working");

PROCESS(send_packets, "");

PROCESS(master_batt, "");

PROCESS_THREAD(master_working, ev, data){
	PROCESS_BEGIN();

	list_init(packet_list);

	// list_add(packet_list,*packet);
	// list_pop(packet_list);
	// list_length(packet_list);

	static struct etimer et;

	process_start(&send_packets,NULL);

	process_start(&master_batt,"");

	PROCESS_PAUSE();

	etimer_set(&et, SEND_TO_CLOUD_INTERVAL*CLOCK_SECOND);

	static int ret = 99;

	process_post(&master_batt,PROCESS_EVENT_CONTINUE,&ret);

	while(true){

		PROCESS_YIELD();

		if(ev == etimer_expired(&et) || (list_length(packet_list) >= CLOUD_PACKAGE_SIZE)){
			// send_packets();
			sens_status = SENS_READY;
			process_poll(&send_packets);
			process_post(&master_batt,PROCESS_EVENT_CONTINUE,&ret);
		}

		// PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));



		etimer_reset(&et);
	}

	PROCESS_END();
}

static bool sensors_initialized = false;
// bool reading_done_flag = false;

PROCESS_THREAD(master_batt, ev, data){
	PROCESS_BEGIN();

	if(!sensors_initialized){
		#if CONTIKI_TARGET_ZOUL
		#else
		sens_battery_initialize();
		#endif
		sensors_initialized = true;
	}

	static uint8_t i;

	static uint16_t battery;

	static int *dat = NULL;

	while(true){
		PROCESS_YIELD();
		dat = data;
		if((ev == PROCESS_EVENT_CONTINUE) && (dat != NULL)){
			if(*dat == 99){
				battery = 0;

				for(i = 0; i < 8; i++){
					#if CONTIKI_TARGET_ZOUL
						// batt = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
						// printf("BATTERY RAW: %u\n",vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED));
						battery += (10.0/55.0)*(vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED)-2750);
						// printf("BATTERY: %u\n",battery);
					#else
						battery += read_battery();
					#endif
				}

				#if CONTIKI_TARGET_ZOUL
					MASTER_BATTERY = (uint8_t) ((battery>>3));
				#else
					// readings.battery = (uint8_t) ((battery>>SR_READINGS)*BATTERY_CONST);
					MASTER_BATTERY = (uint8_t) ((battery>>3)*BATTERY_CONST);
				#endif


				// reading_done_flag = true;
			}
		}
	}
	PROCESS_END();
}

bool send_sensors_packet(void);

PROCESS_THREAD(send_packets, ev, data){
	PROCESS_BEGIN();
	#if CONTIKI_TARGET_ZOUL
	static uint8_t n = 0;
	static uint8_t i;

	extern char cloud_sens[256];

	static struct slave_msg_t *ptr;

	// static struct etimer et_timeout;

	while(true){
		PROCESS_YIELD();



		if((ev == PROCESS_EVENT_POLL) && (sens_status == SENS_READY)){
			n = list_length(packet_list);
			if(n > 3) n = 3;
			sprintf(cloud_sens,"{ \"M\": %u,",m_remote_ID);

			for(i = 0; i < 3; i++){
				if(i < n){
					if(pop_packet(&ptr)){
						sprintf(cloud_sens,"%s\"R%u\": %i,",cloud_sens,i+1,ptr->remote_id);
						sprintf(cloud_sens,"%s\"T%u\": %i.%i,",cloud_sens,i+1,(int)ptr->temperature,((int)(ptr->temperature*100))%100);
						sprintf(cloud_sens,"%s\"H%u\": %i.%i,",cloud_sens,i+1,(int)ptr->humidity,((int)(ptr->humidity*100))%100);
						sprintf(cloud_sens,"%s\"A%u\": %i.%i,",cloud_sens,i+1,(int)ptr->airflow,((int)(ptr->airflow*100))%100);
						sprintf(cloud_sens,"%s\"B%u\": %u,",cloud_sens,i+1,ptr->battery);
					}
				}
				else{
					sprintf(cloud_sens,"%s\"R%u\": %i,",cloud_sens,i+1,-1);
					sprintf(cloud_sens,"%s\"T%u\": %i,",cloud_sens,i+1,-1);
					sprintf(cloud_sens,"%s\"H%u\": %i,",cloud_sens,i+1,-1);
					sprintf(cloud_sens,"%s\"A%u\": %i,",cloud_sens,i+1,-1);
					sprintf(cloud_sens,"%s\"B%u\": %u,",cloud_sens,i+1,-1);
				}
			}

			sprintf(cloud_sens,"%s\"MB\": %u}",cloud_sens,MASTER_BATTERY);

			printf("Send packet:\n%s\n",cloud_sens);

			SEND_MODE = SEND_SENSOR_DATA;

		}
	}
	#endif
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
		if(sens_status == SENS_IDLE){
			printf("Buffer limit!\n");
			sens_status = SENS_READY;
			process_poll(&send_packets);
		}
	}

	return true;
}

bool pop_packet(struct slave_msg_t **packet){
	if(list_length(packet_list) <= 0) return false;

	*packet = list_pop(packet_list);

	printf("%i\n", (*packet)->remote_id);
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
// PROCESS(config_cloudmode_work, "");

PROCESS_THREAD(config_cloudmode_request, ev, data){
    PROCESS_BEGIN();

    #if CONTIKI_TARGET_ZOUL
    static int ret = 10;
    static int *dat = NULL;

    static struct etimer et_timeout;

    while(CLOUD_MODE == STATUS_UNDEFINED){
        PROCESS_YIELD();
        dat = data;
        if((ev == PROCESS_EVENT_MSG) && dat != NULL){
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

	static struct etimer et_timeout;

	static bool ev_started = false;

	while(true){
		PROCESS_YIELD();
		dat = data;
		if((ev == PROCESS_EVENT_MSG) && dat != NULL){
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

	static struct etimer et_timeout;

	while(true){
		PROCESS_YIELD();
		dat = data;

		if((ev == PROCESS_EVENT_MSG) && dat != NULL){
        	if(*dat == 22){
        		printf("Start SEND LOCAL IDS\n");
        		SEND_MODE = SEND_CONFIG_IDS_LOC;
				RECEIVE_MODE = RECEIVE_CONFIG_IDS_LOC;
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

	static struct etimer et_timeout;

	while(true){
		PROCESS_YIELD();
		dat = data;

		if((ev == PROCESS_EVENT_MSG) && dat != NULL){
        	if(*dat == 23){
        		printf("Start RECEIVE REMOTE IDS\n");
        		mote_count = 0;
        		SEND_MODE = SEND_CONFIG_IDS_REM;
				RECEIVE_MODE = RECEIVE_CONFIG_IDS_LOC;
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

	printf("RECEIVE REMOTE IDS DONE\n");

	process_post(&master_config, PROCESS_EVENT_CONTINUE, &ret);

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

	add_MOTE(IEEE_ADDR_NODE_ID);

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

					// Requesting Add Motes
					// printf("DAT == 10\n");
					*ret = 21;
					// printf("Adding Motes\n");
					process_post(&config_cloudmode_conf, PROCESS_EVENT_MSG, ret);
				break;
				case 11:
					// Return from -> Add Motes

					switch(CLOUD_MODE){
						case STATUS_UNDEFINED:
							printf("ERROR! UNDEFINED CLOUDMODE\n");
						break;
						case STATUS_CONFIG:
							// Sending Ids
							// printf("DAT == 11\n");
							*ret = 22;
							process_post(&config_cloudmode_sendLocalIDS, PROCESS_EVENT_MSG, ret);		
						break;
						case STATUS_WORKING:
							// printf("DAT == 12\n");
							*ret = 23;
							// printf("Receiving Remote Ids\n");
							process_post(&config_cloudmode_receiveRemoteIDS, PROCESS_EVENT_MSG, ret);		
						break;
					}
				break;
				case 12:
					*ret = 23;
					// printf("DAT == 12\n");
					process_post(&config_cloudmode_receiveRemoteIDS, PROCESS_EVENT_MSG, ret);
				break;
				case 13:
					// printf("DAT == 13\n");
					printf("Config DONE\n");
					configDONE = true;
				break;
			}
		}
	}

	configDONE = false;

	printf("Changing to Working Mode\n");

	static MODE_t mod;
	mod = WORKING;
	set_state(MODE,&mod);

	#else
	#endif

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
	static struct MOTE_t *aux;
	for(aux = list_head(motes_list);aux != NULL; aux = list_item_next(aux)){
		if(aux->local_id == local_ID){
			printf("UPDATED MOTE\n");
			aux->remote_id = remote_ID;
			#if CONTIKI_TARGET_ZOUL
			if(local_ID == IEEE_ADDR_NODE_ID) m_remote_ID = remote_ID;
			#else
			#endif
			printf("Local ID: %u, Remote ID: %u\n", local_ID, remote_ID);
			return true;
		}
	}
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