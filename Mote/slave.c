#include "slave.h"

#include "contiki.h"
#include "contiki-net.h"
#include "er-coap-engine.h"

#if CONTIKI_TARGET_ZOUL
#include "dev/zoul-sensors.h"
#else /* Assumes the Z1 mote */
#include "sens_temperature.h"
#include "sens_humidity.h"
#include "sens_airflow.h"
#include "sens_battery.h"
#include <sys/node-id.h>
#endif

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"

#include "readJSON.h"
#include "msg.h"

bool ID_sended = false;
uint8_t remote_ID = 0;

#define LOCAL_PORT      UIP_HTONS(COAP_DEFAULT_PORT + 1)
#define REMOTE_PORT     UIP_HTONS(COAP_DEFAULT_PORT)
#define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0xfd00, 0, 0, 0, 0, 0, 0, 0x1) // Address of VM on TunSlip6 - fd00::1 

/*---------------------------*/
/*----------Working----------*/
/*---------------------------*/


static struct sensor_data readings;

bool reading_done_flag = false;

PROCESS(slave_working, "Slave Workinig");
PROCESS(take_readings, "Readings");

PROCESS_THREAD(slave_working, ev, data){
	PROCESS_BEGIN();

	static struct etimer et;
	static struct etimer et_timeout;

	etimer_set(&et, READINGS_TIME*CLOCK_SECOND);
	etimer_set(&et_timeout, TIMEOUT*CLOCK_SECOND);

	static bool timeout_flag;
	static int Ntry;

	Ntry = 0;

	printf("Slave Workinig\n");

	while(true){

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		reading_done_flag = false;
		timeout_flag = false;

		process_start(&take_readings,"");

		etimer_reset(&et_timeout);

		while(reading_done_flag == false){
			PROCESS_YIELD();
			if((ev = etimer_expired(&et_timeout))){
				printf("TIMEOUT\n");
				Ntry++;
				if(Ntry >= MTRY){
					timeout_flag = true;
					break;
				}
				else{
					process_start(&take_readings,"");
					etimer_reset(&et_timeout);
				}
			}
		}

		if(!timeout_flag){
			printf("Temperature: %i, Humidity: %i, AriFlow: %i, Battery: %u\n", (int)(readings.temperature),
																				(int)(readings.humidity),
																				(int)(readings.airflow),
																				readings.battery);

		}
		
		etimer_reset(&et);
	}

	PROCESS_END();
}

bool sensors_initialized = false;

PROCESS_THREAD(take_readings, ev, data){
	PROCESS_BEGIN();

	if(!sensors_initialized){
		#if CONTIKI_TARGET_ZOUL

		#else
		sens_temperature_initialize();
		sens_humidity_initialize();
		sens_airflow_initialize();
		sens_battery_initialize();
		#endif
	}

	static uint8_t i;

	#if CONTIKI_TARGET_ZOUL

	#else
		static uint32_t temperature;
		static uint32_t humidity;
		static uint32_t airflow;
		static uint16_t battery;

		temperature = 0;
		humidity = 0;
		airflow = 0;
		battery = 0;
	#endif

	for(i = 0; i < N_READINGS; i++){
		#if CONTIKI_TARGET_ZOUL
		#else
			temperature += read_temperature();
			humidity += read_humidity();
			airflow += read_airflow();
			battery += read_battery();
		#endif
	}

	#if CONTIKI_TARGET_ZOUL
	#else
		readings.temperature = (float) (temperature>>SR_READINGS);
		readings.humidity = (float) ((humidity>>SR_READINGS)*HUMIDITY_CONST);
		readings.airflow = (float) (airflow>>SR_READINGS);
		readings.battery = (uint8_t) ((battery>>SR_READINGS)*BATTERY_CONST);
	#endif


	// printf("Temperature: %i\n",(int)readings.temperature);
	// printf("Humidity: %u%%\n",(int)readings.humidity);
	// printf("Airflow: %u\n",(int)readings.airflow);
	// printf("Battery: %u%%\n",readings.battery);
	reading_done_flag = true;
	PROCESS_END();
}

void exec_slave_working(void){
	process_start(&slave_working,"");
}

void exit_slave_working(void){
	if(process_is_running(&take_readings)!=0){
		process_exit(&take_readings);	
	}
	process_exit(&slave_working);
}

/*--------------------------*/
/*----------Config----------*/
/*--------------------------*/

static uip_ipaddr_t server_ipaddr;



void config_post_handler(void *response);

void config_get_handler(void *response);

PROCESS(slave_config, "Slave Config");
PROCESS(slave_config_get,"");

#undef DEBUG
#define DEBUG 1

PROCESS_THREAD(slave_config, ev, data){
	PROCESS_BEGIN();
	process_start(&slave_config_get, &data);

  	PROCESS_PAUSE();
	// CoAP Initialization
	static struct etimer et_timeout;
  	static coap_packet_t request[1];
  	static coap_packet_t request_2[1];
  	SERVER_NODE(&server_ipaddr);
  	coap_init_engine();

	etimer_set(&et_timeout, TIMEOUT * CLOCK_SECOND);

  	static char msg[128];

	coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);

	char *confgURL = "config";

	coap_set_header_uri_path(request, confgURL);

	printf(confgURL);
	printf("\n");

#if CONTIKI_TARGET_ZOUL
	sprintf(msg,"{\"ID\": %u}\n",IEEE_ADDR_NODE_ID);
#else
	sprintf(msg,"{\"ID\": %u}\n",node_id);
#endif

	coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

	uint8_t count_post = 0;

	while(!ID_sended){
		COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request,
		                    config_post_handler);
	}

  	printf("ID sent. Waiting for remote ID.\n");

	process_poll(&slave_config_get);

  	while(remote_ID == 0){
  		PROCESS_YIELD();
  	}

	printf("Remote ID received.\n");

	PROCESS_END();
}


PROCESS_THREAD(slave_config_get, ev, data){
	PROCESS_BEGIN();
  	static coap_packet_t request[1];

	while(remote_ID == 0){
		PROCESS_YIELD();
		if(ev == PROCESS_EVENT_POLL) {
			coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
			coap_set_header_uri_path(request, "config");

			char query[15];

			#if CONTIKI_TARGET_ZOUL
				sprintf(query,"?ID=%u",IEEE_ADDR_NODE_ID);
			#else
				sprintf(query,"?ID=%u",node_id);
			#endif

			coap_set_header_uri_query(request,query);

			while(remote_ID == 0){
				COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request,
			                config_get_handler);	
			}
		}
	}
	PROCESS_END();
}



void exec_slave_config(void){
	process_start(&slave_config,"");
}

void exit_slave_config(void){
	process_exit(&slave_config);
}


void config_post_handler(void *response){
  int len = 0;
  const uint8_t *payload = NULL;

  printf("Post Handler\n");

  coap_get_payload(response, &payload);
  printf("Post Handler: %s\n", payload);

  static int params[1];

  readJSON_uf(payload, params,NULL);

  if((params[0] == 1) || (params[0] == -2)) ID_sended = true;
  else ID_sended = false;
}

void config_get_handler(void *response){
  int len = 0;
  const uint8_t *payload = NULL;

  coap_get_payload(response, &payload);
  printf("Get Handler: %s\n", (char *)payload);

  static int params[1];

  readJSON_i(payload, params);

  printf("Response: %i\n", params[0]);
  
  if((params[0] >= 1) && (params[0] <= 255)) remote_ID = params[0];
  else remote_ID = 0;
}

