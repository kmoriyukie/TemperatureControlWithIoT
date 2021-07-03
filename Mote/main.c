#include "contiki.h"

#include "events_threads.h"
#include "states.h"

extern struct process border_router_process; //  /Connection/BorderRouter.c

// extern struct process MQTTServerProcess; //  /Connection/BorderRouter.c

extern struct process button_pressed;

PROCESS(main_process,"main_proc");
AUTOSTART_PROCESSES(&main_process,&button_pressed);

PROCESS_THREAD(main_process, ev, data){
  	PROCESS_BEGIN();

  	process_start(&border_router_process, "Border Router process");

  	// process_start(&MQTTServerProcess, "MQTT Server process");

  	initialize_states(&event_blink_leds);

	PROCESS_END();
}
