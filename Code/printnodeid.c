#include "contiki.h"

#include <stdio.h> /* For printf() */

#if !CONTIKI_TARGET_ZOUL
#include "sys/node-id.h"
#endif


/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();

  printf("Hello, world\n");

#if !CONTIKI_TARGET_ZOUL
  printf("0x%02x\n", node_id);
#endif

#ifdef IEEE_ADDR_NODE_ID
  printf("0x%02x\n", IEEE_ADDR_NODE_ID);
#endif
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
