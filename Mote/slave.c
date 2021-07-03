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

#include "msg.h"

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
	etimer_set(&et_timeout, TIMOUT*CLOCK_SECOND);

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

#define REMOTE_PORT     UIP_HTONS(COAP_DEFAULT_PORT)
#define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0xfd00, 0, 0, 0, 0, 0, 0, 0x1) // Address of VM on TunSlip6 - fd00::1 

void client_chunk_handler(void *response);

PROCESS(slave_config, "Slave Config");

PROCESS_THREAD(slave_config, ev, data){
	PROCESS_BEGIN();

	// CoAP Initialization
	static struct etimer et_timeout;
  	static coap_packet_t request[1];
  	SERVER_NODE(&server_ipaddr);
  	coap_init_engine();

  	etimer_set(&et_timeout, TIMOUT * CLOCK_SECOND);

  	static char msg[128];

  	while(true) {
  		PROCESS_YIELD();

  		if(etimer_expired(&et_timeout)){
  			coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);

			char *fullURL = "config";

			coap_set_header_uri_path(request, fullURL);

			printf(fullURL);
			printf("\n");

			#if CONTIKI_TARGET_ZOUL
				sprintf(msg,"{\"ID\": %u}\0",IEEE_ADDR_NODE_ID);
			#else
				sprintf(msg,"{\"ID\": %u}\0",node_id);
			#endif

			coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

			COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request,
			                    client_chunk_handler);

			etimer_reset(&et_timeout);
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



// PROCESS(CoAP_Client, "CoAP Client");
// AUTOSTART_PROCESSES(&er_example_client);

// uip_ipaddr_t server_ipaddr;

// #define NUMBER_OF_URLS 4

void client_chunk_handler(void *response){
  int len = 0;
  const uint8_t *payload = NULL;

  coap_get_payload(response, &payload);

  printf("Payload: %s\n", payload);

  //Stop Condition

  /*
  // REST.get_response_payload(response, "asd", 16);

  printf("Reply Received! Code: %d\n",returnCode);
  if(returnCode==COAP_RESPONSE_NOT_FOUND){
    printf("Service Not Found!\n\n");
  }else{
    if(returnCode==COAP_RESPONSE_OK){
      printf("OK!\n");
    }else{
      printf("Server Error!\n\n");
    }
  }*/
}

// PROCESS_THREAD(CoAP_Client, ev, data)
// {
//   PROCESS_BEGIN();

//   static struct etimer et;

//   static coap_packet_t request[1];      /* This way the packet can be treated as pointer as usual. */

//   SERVER_NODE(&server_ipaddr);

//   /* receives all CoAP messages */
//   coap_init_engine();

//   //etimer_set(&et, TOGGLE_INTERVAL * CLOCK_SECOND);

//     // SENSORS_ACTIVATE(button_sensor);

//   char msg[100];
//   uint8_t mode = 0;


//   while(1) {
//     PROCESS_YIELD();

//     if(ev == sensors_event && data == &button_sensor) {

//       printf("--Toggle timer--\n");

//       //coap_init_message(request, COAP_TYPE_CON, COAP_PUT, 0);
//       //coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);

//       coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);

//       char *fullURL = "actuators/toggle";
//       //char *fullURL = "actuators/leds";
//       coap_set_header_uri_path(request, fullURL);
//       //coap_set_header_uri_query(request,"?len=10");

//       printf(fullURL);
//       printf("\n");

//       //sprintf(msg,"mode=on");
//       //coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

//       COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request,
//                             client_chunk_handler);

//       printf("\n--Done--\n");

//       etimer_reset(&et);
//     }

//   }

//   PROCESS_END();
// }
