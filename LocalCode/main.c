#include "contiki.h"

#include "events_threads.h"
#include "states.h"

extern ROLE_t node_role;
extern MODE_t node_mode;

PROCESS(main_process,"main_proc");
AUTOSTART_PROCESSES(&main_process);

PROCESS_THREAD(main_process, ev, data){
	static struct etimer et;

	initialize_states(&event_blink_leds);

  	PROCESS_BEGIN();

  	set_role(SLAVE);
  	set_mode(CONFIG);

  	

	// etimer_set(&et, 10*CLOCK_SECOND);
	// PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	// event_blink_leds(0b101);

	PROCESS_END();
}

// #include "contiki.h"

// #include "net/ip/uip.h"
// #include "net/ipv6/uip-ds6.h"

// /* This is quite handy, allows to print IPv6 related stuff in a readable way */
// #include "net/ip/uip-debug.h"

// /* The simple UDP library API */
// #include "simple-udp.h"

// /* Library used to read the metadata in the packets */
// #include "net/packetbuf.h"

// /* And we are including the example.h with the example configuration */
// #include "../msg.h"
// #include "sys/node-id.h"


// #include "dev/adxl345.h"
// #include "dev/battery-sensor.h"
// #include "dev/i2cmaster.h"
// #include "dev/tmp102.h"

// /* And this you should be familiar with from the basic lessons... */
// #include "sys/etimer.h"
// #include "dev/leds.h"

// #include <stdio.h>
// #include <string.h>
// /*---------------------------------------------------------------------------*/
// #define SEND_INTERVAL (5 * CLOCK_SECOND)
// /*---------------------------------------------------------------------------*/
// /* The structure used in the Simple UDP library to create an UDP connection */
// static struct simple_udp_connection mcast_connection;
// /*---------------------------------------------------------------------------*/
// /* Create a structure and pointer to store the data to be sent as payload */
// static struct my_msg_t msg;
// static struct my_msg_t *msgPtr = &msg;
// static uint8_t flag = 0;
// /*---------------------------------------------------------------------------*/
// PROCESS(mcast_example_process, "UDP multicast example process");
// AUTOSTART_PROCESSES(&mcast_example_process);
// /*---------------------------------------------------------------------------*/
// /* This is the receiver callback, we tell the Simple UDP library to call this
//  * function each time we receive a packet
//  */

// static void showMSG(struct my_msg_t *msgP){
//   printf("Message: %s--> %u\n",msgP->text,node_id);
// }

// static void propagateMSG(struct my_msg_t *msgP){
//   msg.id = node_id;

//   sprintf(msg.text,"%s--> %u ",msgP->text,node_id);
// }

// static void receiver(struct simple_udp_connection *c,
//          const uip_ipaddr_t *sender_addr,
//          uint16_t sender_port,
//          const uip_ipaddr_t *receiver_addr,
//          uint16_t receiver_port,
//          const uint8_t *data,
//          uint16_t datalen)
// {
//   radio_value_t aux;

//   struct my_msg_t *msgPtr = (struct my_msg_t *) data;

//   leds_toggle(LEDS_GREEN);

//   if(msgPtr->id < node_id){
//       showMSG(msgPtr);
//       propagateMSG(msgPtr);
//       flag = 1;
//   }
// }
// /*---------------------------------------------------------------------------*/
// static void
// set_radio_default_parameters(void)
// {

//   NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, TX_POWER);
//   NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, CHANNEL);
// }
// /*---------------------------------------------------------------------------*/
// PROCESS_THREAD(mcast_example_process, ev, data)
// {
//   static struct etimer periodic_timer;
//   uip_ipaddr_t addr;

//   PROCESS_BEGIN();

//   set_radio_default_parameters();

//   simple_udp_register(&mcast_connection, UDP_CLIENT_PORT, NULL,
//                       UDP_CLIENT_PORT, receiver);

//   etimer_set(&periodic_timer, SEND_INTERVAL);

//   while(1) {
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

//     uip_create_linklocal_allnodes_mcast(&addr);


//     if(node_id == 1){
//       msg.id = node_id;
//       sprintf(msg.text,"Message from node %u ",node_id);
//       simple_udp_sendto(&mcast_connection, msgPtr, sizeof(msg), &addr);
//     }

//     if(flag == 1)
//       simple_udp_sendto(&mcast_connection, msgPtr, sizeof(msg), &addr);

//     flag = 0;
//     etimer_reset(&periodic_timer);
//   }

//   PROCESS_END();
// }
// /*---------------------------------------------------------------------------*/
