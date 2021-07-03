/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 *         Slip fallback interface
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         Joel Hoglund <joel@sics.se>
 *         Nicolas Tsiftes <nvt@sics.se>
 */

#include "mqttMacros.h"

#include "mqtt_states.h"

//#include "res_config_mqtt.c"

extern receive_mqtt_t RECEIVE_MODE;
extern send_mqtt_t SEND_MODE;

static char client_id[BUFFER_SIZE];
char *pub_topic = DEFAULT_PUBLISH_TOPIC;
char *sub_topic = DEFAULT_SUBSCRIBE_TOPIC;

static struct mqtt_connection conn;
static char app_buffer[APP_BUFFER_SIZE];

static struct mqtt_message *msg_ptr = 0;
static struct etimer publish_periodic_timer;
static struct ctimer ct;
static char *buf_ptr;

static mqtt_client_config_t conf;

void pub_handler(const char *topic, uint16_t topic_len, const uint8_t *chunk,
            uint16_t chunk_len)
{
  printf("Pub Handler: topic='%s' (len=%u), chunk_len=%u, content: %s\n", topic, topic_len, chunk_len, chunk);
  

  if(strcmp((char *)topic,CONFIG_CLOUDMODE_TOPIC)==0){
    printf("%s\n", topic);
    receive_cloudmode();
    return;
  }

  if(strcmp((char *)topic,CONFIG_ID_TOPIC)==0){
    printf("%s\n", topic);
    receive_ids();
    return;
  }

}

void mqtt_event(struct mqtt_connection *m, mqtt_event_t event, void *data)
{
  switch(event) {
  case MQTT_EVENT_CONNECTED: {
    printf("APP - Application has a MQTT connection\n");
    timer_set(&connection_life, CONNECTION_STABLE_TIME);
    state = STATE_CONNECTED;
    break;
  }
  case MQTT_EVENT_DISCONNECTED: {
    printf("APP - MQTT Disconnect. Reason %u\n", *((mqtt_event_t *)data));

    state = STATE_DISCONNECTED;
    process_poll(&MQTTServerProcess);
    break;
  }
  case MQTT_EVENT_PUBLISH: {
    msg_ptr = data;
    if(msg_ptr->first_chunk) {
      msg_ptr->first_chunk = 0;
      printf("APP - Application received a publish on topic '%s'. Payload "
          "size is %i bytes.\n",
          msg_ptr->topic, msg_ptr->payload_length);
    }

    pub_handler(msg_ptr->topic, strlen(msg_ptr->topic), msg_ptr->payload_chunk,
                msg_ptr->payload_length);
    break;
  }
  case MQTT_EVENT_SUBACK: {
    printf("APP - Application is subscribed to topic successfully\n");
    break;
  }
  case MQTT_EVENT_UNSUBACK: {
    printf("APP - Application is unsubscribed to topic successfully\n");
    break;
  }
  case MQTT_EVENT_PUBACK: {
    printf("APP - Publishing complete.\n");
    break;
  }
  default:
    printf("APP - Application got a unhandled MQTT event: %i\n", event);
    break;
  }
}

/*---------------------------------------------------------------------------*/
int construct_client_id(void)
{
  int len = snprintf(client_id, BUFFER_SIZE, "d:%02x%02x%02x%02x%02x%02x",
                     linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
                     linkaddr_node_addr.u8[2], linkaddr_node_addr.u8[5],
                     linkaddr_node_addr.u8[6], linkaddr_node_addr.u8[7]);

  if(len < 0 || len >= BUFFER_SIZE) {
    printf("Client ID: %d, Buffer %d\n", len, BUFFER_SIZE);
    return 0;
  }

  return 1;
}

/*---------------------------------------------------------------------------*/
void update_config(void)
{
  if(construct_client_id() == 0) {
    state = STATE_CONFIG_ERROR;
    printf("Fatal error. Client ID larger than the buffer\n");
    return;
  }

  state = STATE_INIT;

  etimer_set(&publish_periodic_timer, 0);

  return;
}

/*---------------------------------------------------------------------------*/
int init_config()
{
  memset(&conf, 0, sizeof(mqtt_client_config_t));
  memcpy(conf.broker_ip, broker_ip, strlen(broker_ip));

  conf.broker_port = DEFAULT_BROKER_PORT;
  conf.pub_interval = DEFAULT_PUBLISH_INTERVAL;

  return 1;
}
/*---------------------------------------------------------------------------*/
void connect_to_broker(void)
{
  mqtt_connect(&conn, conf.broker_ip, conf.broker_port,
               conf.pub_interval * 3);

  state = STATE_CONNECTING;
}
/*---------------------------------------------------------------------------*/
void subscribe(void)
{
  mqtt_status_t status;

  status = mqtt_subscribe(&conn, NULL, sub_topic, MQTT_QOS_LEVEL_0);

  printf("APP - Subscribing to %s\n", sub_topic);
  if(status == MQTT_STATUS_OUT_QUEUE_FULL) {
    printf("APP - Tried to subscribe but command queue was full!\n");
  }

  status = mqtt_subscribe(&conn, NULL, CONFIG_CLOUDMODE_TOPIC, MQTT_QOS_LEVEL_0);

  printf("APP - Subscribing to %s\n", CONFIG_CLOUDMODE_TOPIC);
  if(status == MQTT_STATUS_OUT_QUEUE_FULL) {
    printf("APP - Tried to subscribe but command queue was full!\n");
  }

  status = mqtt_subscribe(&conn, NULL, CONFIG_ID_TOPIC, MQTT_QOS_LEVEL_0);

  printf("APP - Subscribing to %s\n", CONFIG_ID_TOPIC);
  if(status == MQTT_STATUS_OUT_QUEUE_FULL) {
    printf("APP - Tried to subscribe but command queue was full!\n");
  }
}
/*---------------------------------------------------------------------------*/
static void publish_test(void)
{
  int len;
  int remaining = APP_BUFFER_SIZE;

  buf_ptr = app_buffer;

  len = snprintf(buf_ptr, remaining,
                 "{"
                 "\"Id\":\"%u\","
                 "\"Timestamp\":\"%ld\"",
                 IEEE_ADDR_NODE_ID, clock_seconds()*1000);

  if(len < 0 || len >= remaining) {
    printf("Buffer too short. Have %d, need %d + \\0\n", remaining, len);
    return;
  }

  remaining -= len;
  buf_ptr += len;

/*aux = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
  len = snprintf(buf_ptr, remaining, ",\"Core Temp\":\"%u.%02u\"", aux / 1000, aux % 1000);

  remaining -= len;
  buf_ptr += len;*/

  len = snprintf(buf_ptr, remaining, "}");

  if(len < 0 || len >= remaining) {
    printf("Buffer too short. Have %d, need %d + \\0\n", remaining, len);
    return;
  }

  mqtt_publish(&conn, NULL, pub_topic, (uint8_t *)app_buffer,
               strlen(app_buffer), MQTT_QOS_LEVEL_0, MQTT_RETAIN_OFF);

  printf("APP - Publish to %s: %s\n", pub_topic, app_buffer);
}

/*---------------------------------------------------------------------------*/
static void state_machine(void)
{
  switch(state) {
  case STATE_INIT:
    mqtt_register(&conn, &MQTTServerProcess, client_id, mqtt_event,
                  MAX_TCP_SEGMENT_SIZE);

    conn.auto_reconnect = 0;
    connect_attempt = 1;

    state = STATE_REGISTERED;

  case STATE_REGISTERED:
    if(uip_ds6_get_global(ADDR_PREFERRED) != NULL) {
      printf("Registered. Connect attempt %u\n", connect_attempt);
      connect_to_broker();
    }
    else {
      ctimer_set(&ct, NO_NET_LED_DURATION, NULL, NULL);
    }
    etimer_set(&publish_periodic_timer, NET_CONNECT_PERIODIC);
    return;
    break;

  case STATE_CONNECTING:
    leds_on(LEDS_GREEN);
    ctimer_set(&ct, CONNECTING_LED_DURATION, NULL, NULL);
    printf("Connecting (%u)\n", connect_attempt);
    break;

  case STATE_CONNECTED:
  case STATE_PUBLISHING:
    if(timer_expired(&connection_life)) {
      connect_attempt = 0;
    }

    if(mqtt_ready(&conn) && conn.out_buffer_sent) {
      if(state == STATE_CONNECTED) {
        subscribe(); //should only be ran once, when the state is connected
        state = STATE_PUBLISHING;

      } else {
        // ctimer_set(&ct, PUBLISH_LED_ON_DURATION, NULL, NULL);
        //Publish Here
        // publish_test();

        switch(SEND_MODE){
          case SEND_NONE:
          break;
          case SEND_CONFIG_CLOUDMODE_REQUEST:
            send_cloudmode();
          break;
          case SEND_CONFIG_IDS_REMLOC: case SEND_CONFIG_IDS_ACK:
            send_ids();
          break;
        }
      }
      etimer_set(&publish_periodic_timer, conf.pub_interval);

      return;

    } else {
      printf("Publishing... (MQTT state=%d, q=%u)\n", conn.state,
          conn.out_queue_full);
    }
    break;

  case STATE_DISCONNECTED:
    printf("Disconnected\n");
    if(connect_attempt < RECONNECT_ATTEMPTS ||
       RECONNECT_ATTEMPTS == RETRY_FOREVER) {
      clock_time_t interval;
      mqtt_disconnect(&conn);
      connect_attempt++;

      interval = connect_attempt < 3 ? RECONNECT_INTERVAL << connect_attempt :
        RECONNECT_INTERVAL << 3;

      printf("Disconnected. Attempt %u in %lu ticks\n", connect_attempt, interval);

      etimer_set(&publish_periodic_timer, interval);

      state = STATE_REGISTERED;
      return;

    } else {
      state = STATE_ERROR;
      printf("Aborting connection after %u attempts\n", connect_attempt - 1);
    }
    break;

  case STATE_CONFIG_ERROR:
    printf("Bad configuration.\n");
    return;

  case STATE_ERROR:
  default:
    printf("Default case: State=0x%02x\n", state);
    return;
  }

  etimer_set(&publish_periodic_timer, STATE_MACHINE_PERIODIC);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(MQTTServerProcess, ev, data)
{
  PROCESS_BEGIN();

  PROCESS_PAUSE();

  if(init_config() != 1) {
    PROCESS_EXIT();
  }

  update_config();

  while(1) {
    PROCESS_YIELD();

    if((ev == PROCESS_EVENT_TIMER && data == &publish_periodic_timer) ||
       ev == PROCESS_EVENT_POLL) {
       state_machine();
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/