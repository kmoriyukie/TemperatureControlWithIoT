#include "contiki.h"

#include "events_threads.h"
#include "states.h"

// extern ROLE_t node_role;
// extern MODE_t node_mode;

extern struct process button_pressed;

PROCESS(main_process,"main_proc");
AUTOSTART_PROCESSES(&main_process,&button_pressed);

PROCESS_THREAD(main_process, ev, data){
  	PROCESS_BEGIN();

  	initialize_states(&event_blink_leds);



	PROCESS_END();
}
