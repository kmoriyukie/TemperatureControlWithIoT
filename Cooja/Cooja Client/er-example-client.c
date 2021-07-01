#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "er-coap-engine.h"
#include "dev/button-sensor.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/* FIXME: This server address is hard-coded for Cooja and link-local for unconnected border router. */
#define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0xfd00, 0, 0, 0, 0, 0, 0, 0x1) // Address of VM on TunSlip6 - fd00::1 
//#define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0xfd00, 0,0, 0, 0x212, 0x4b00, 0x89ab, 0xcdef) 

#define LOCAL_PORT      UIP_HTONS(COAP_DEFAULT_PORT + 1)
#define REMOTE_PORT     UIP_HTONS(COAP_DEFAULT_PORT)

#define COAP_RESPONSE_OK             69
#define COAP_RESPONSE_NOT_FOUND     132

#define TOGGLE_INTERVAL 10

PROCESS(er_example_client, "Erbium Example Client");
AUTOSTART_PROCESSES(&er_example_client);

uip_ipaddr_t server_ipaddr;
static struct etimer et;

#define NUMBER_OF_URLS 4

void client_chunk_handler(void *response)
{
  int returnCode = ((coap_packet_t  *)response)->code;

  printf("Reply Received! Code: %d\n",returnCode);
  if(returnCode==COAP_RESPONSE_NOT_FOUND){
    printf("Service Not Found!\n\n");
  }else{
    if(returnCode==COAP_RESPONSE_OK){
      printf("OK!\n");
    }else{
      printf("Server Error!\n\n");
    }
  }
}

PROCESS_THREAD(er_example_client, ev, data)
{
  PROCESS_BEGIN();

  static coap_packet_t request[1];      /* This way the packet can be treated as pointer as usual. */

  SERVER_NODE(&server_ipaddr);

  /* receives all CoAP messages */
  coap_init_engine();

  //etimer_set(&et, TOGGLE_INTERVAL * CLOCK_SECOND);

    SENSORS_ACTIVATE(button_sensor);

  char msg[100];
  uint8_t mode = 0;


  while(1) {
    PROCESS_YIELD();

    if(ev == sensors_event && data == &button_sensor) {

      printf("--Toggle timer--\n");

      //coap_init_message(request, COAP_TYPE_CON, COAP_PUT, 0);
      //coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);

      coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);

      char *fullURL = "actuators/toggle";
      //char *fullURL = "actuators/leds";
      coap_set_header_uri_path(request, fullURL);
      //coap_set_header_uri_query(request,"?len=10");

      printf(fullURL);
      printf("\n");

      //sprintf(msg,"mode=on");
      //coap_set_payload(request, (uint8_t *)msg, sizeof(msg) - 1);

      COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request,
                            client_chunk_handler);

      printf("\n--Done--\n");

      etimer_reset(&et);
    }

  }

  PROCESS_END();
}
