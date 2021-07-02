#include "contiki.h"

extern struct process border_router_process;
extern struct process coap_server_process;
extern struct process MQTTServerProcess;

PROCESS(main_process, "Main process");

extern struct process;

AUTOSTART_PROCESSES(&main_process);

PROCESS_THREAD(main_process, ev, data)
{
	PROCESS_BEGIN();

	process_start(&border_router_process, "Border Router process");
	process_start(&coap_server_process, "CoAP process");
	process_start(&MQTTServerProcess, "MQTT Process");
	while(1){
		PROCESS_YIELD();
	}

	PROCESS_END();
}