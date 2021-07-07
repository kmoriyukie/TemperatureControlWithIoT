#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"

extern resource_t
  res_config,
  res_sensors;

PROCESS(coap_server_process, "CoAP server process");

PROCESS_THREAD(coap_server_process, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  rest_init_engine();

  // rest_activate_resource(&res_toggle, "actuators/toggle");  
  printf("%s\n", (char *) data);
  rest_activate_resource(&res_config, "config");  
  rest_activate_resource(&res_sensors, "sensors");  

  // printf("\nCoAP server started\n");


  // while(1) {
  //   PROCESS_WAIT_EVENT();
  // }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
