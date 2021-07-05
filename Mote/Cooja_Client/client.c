#include "contiki.h"
#include <stdio.h>
#include "../events_threads.h"
#include "../states.h"

extern struct process button_pressed;

PROCESS(main_process,"main_proc");
AUTOSTART_PROCESSES(&main_process,&button_pressed);

PROCESS_THREAD(main_process, ev, data){
  	PROCESS_BEGIN();

  	printf("%u\n", UIP_CONF_BUFFER_SIZE);
  	initialize_states(&event_blink_leds);

	PROCESS_END();
}
