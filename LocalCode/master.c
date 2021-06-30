#include "master.h"

#include "contiki.h"
#include "list.h"

#if CONTIKI_TARGET_ZOUL
#include "dev/zoul-sensors.h"
#else /* Assumes the Z1 mote */
#include "sens_battery.h"
#endif

#include "msg.h"
#include "stdbool.h"

/*---------------------------*/
/*----------Working----------*/
/*---------------------------*/

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




/*--------------------------*/
/*----------Config----------*/
/*--------------------------*/

PROCESS(master_config, "Master Config");

PROCESS_THREAD(master_config, ev, data){
	PROCESS_BEGIN();



	PROCESS_END();
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
