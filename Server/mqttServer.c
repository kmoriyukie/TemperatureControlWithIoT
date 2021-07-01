#include "mqttMacros.h"

static char client_id[BUFFER_SIZE];
static char pub_topic[BUFFER_SIZE];
static char sub_topic[BUFFER_SIZE];

static struct mqtt_connection conn;
static char app_buffer[APP_BUFFER_SIZE];

static struct mqtt_message *msg_ptr = 0;
static struct etimer publish_periodic_timer;
static struct ctimer ct;
static char *buf_ptr;
static uint16_t seq_nr_value = 0;

static mqtt_client_config_t conf;


void readJSON(const char *json, float *params, const int nParams){
    const static char sep1[] = ":";
    const static char sep2[] = ",";
    const static char sep3[] = "}";
    char *ptr1;
    char *ptr2;
    char buff[16];
    
    int i,j;
    for(i = 0; i < nParams; i++){
        if(i == 0){
            ptr1 = strstr(json,sep1);
            ptr2 = strstr(json,sep2);
        }
        else{
            if(i == nParams-1) ptr2 = strstr(ptr2+1,sep3);
            else ptr2 = strstr(ptr2+1,sep2);
        }
        memset(buff,0,16);
        for(j = 2; j < (ptr2-ptr1); j++){
            buff[j-2] = ptr1[j];
        }
        params[i] = atof(buff);
        ptr1 = strstr(ptr1+1,sep1);
    }
}
/*---------------------------------------------------------------------------*/
static void pub_handler(const char *topic, uint16_t topic_len, const uint8_t *chunk,
            uint16_t chunk_len)
{
  printf("Pub Handler: topic='%s' (len=%u), chunk_len=%u, content: %s\n", topic, topic_len, chunk_len, chunk);
  
  if(strcmp((char *)topic,DEFAULT_PUBLISH_TOPIC)==0){ //=== ex 2
    printf("%s\n", topic);  
    if (strcmp((char *)chunk,"1")==0) leds_on(LEDS_RED);
    else if(strcmp((char *)chunk,"0")==0) leds_off(LEDS_RED);
  }

  return;
}

/*---------------------------------------------------------------------------*/
static void publish_led_off(void *d)
{
  leds_off(LEDS_GREEN);
}

/*---------------------------------------------------------------------------*/
static void mqtt_event(struct mqtt_connection *m, mqtt_event_t event, void *data)
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

    /* Implement first_flag in publish message? */
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
static int construct_pub_topic(void)
{
  int len = snprintf(pub_topic, BUFFER_SIZE, DEFAULT_PUBLISH_TOPIC);
  if(len < 0 || len >= BUFFER_SIZE) {
    printf("Pub Topic too large: %d, Buffer %d\n", len, BUFFER_SIZE);
    return 0;
  }

  printf("Publishing topic: %s\n", pub_topic);

  return 1;
}
/*---------------------------------------------------------------------------*/
static int construct_sub_topic(void)
{
  int len = snprintf(sub_topic, BUFFER_SIZE, DEFAULT_SUBSCRIBE_TOPIC);
  if(len < 0 || len >= BUFFER_SIZE) {
    printf("Sub Topic too large: %d, Buffer %d\n", len, BUFFER_SIZE);
    return 0;
  }

  printf("Subscription topic: %s\n", sub_topic);

  return 1;
}
/*---------------------------------------------------------------------------*/
static int construct_client_id(void)
{
  int len = snprintf(client_id, BUFFER_SIZE, "d:%02x%02x%02x%02x%02x%02x",
                     linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
                     linkaddr_node_addr.u8[2], linkaddr_node_addr.u8[5],
                     linkaddr_node_addr.u8[6], linkaddr_node_addr.u8[7]);

  /* len < 0: Error. Len >= BUFFER_SIZE: Buffer too small */
  if(len < 0 || len >= BUFFER_SIZE) {
    printf("Client ID: %d, Buffer %d\n", len, BUFFER_SIZE);
    return 0;
  }

  return 1;
}

/*---------------------------------------------------------------------------*/
static void update_config(void)
{
  if(construct_client_id() == 0) {
    state = STATE_CONFIG_ERROR;
    printf("Fatal error. Client ID larger than the buffer\n");
    return;
  }

  if(construct_sub_topic() == 0) {
    state = STATE_CONFIG_ERROR;
    printf("Fatal error. Sub topic larger than the buffer\n");
    return;
  }  

  if(construct_pub_topic() == 0) {
    state = STATE_CONFIG_ERROR;
    printf("Fatal error. Pub topic larger than the buffer\n");
    return;
  }

  seq_nr_value = 0;

  state = STATE_INIT;

  etimer_set(&publish_periodic_timer, 0);

  return;
}

/*---------------------------------------------------------------------------*/
static int init_config()
{
  memset(&conf, 0, sizeof(mqtt_client_config_t));
  memcpy(conf.event_type_id, DEFAULT_PUBLISH_TOPIC, strlen(DEFAULT_PUBLISH_TOPIC));
  memcpy(conf.broker_ip, broker_ip, strlen(broker_ip));
  memcpy(conf.cmd_type, DEFAULT_SUBSCRIBE_TOPIC, strlen(DEFAULT_SUBSCRIBE_TOPIC)); //receive data

  conf.broker_port = DEFAULT_BROKER_PORT;
  conf.pub_interval = DEFAULT_PUBLISH_INTERVAL;

  return 1;
}
/*---------------------------------------------------------------------------*/
static void connect_to_broker(void)
{
  mqtt_connect(&conn, conf.broker_ip, conf.broker_port,
               conf.pub_interval * 3);

  state = STATE_CONNECTING;
}
/*---------------------------------------------------------------------------*/
static void subscribe(void)
{
  mqtt_status_t status;

  status = mqtt_subscribe(&conn, NULL, sub_topic, MQTT_QOS_LEVEL_0);

  printf("APP - Subscribing to %s\n", sub_topic);
  if(status == MQTT_STATUS_OUT_QUEUE_FULL) {
    printf("APP - Tried to subscribe but command queue was full!\n");
  }
}
/*---------------------------------------------------------------------------*/
static void publish_temperature(void)
{
  int len;
  int remaining = APP_BUFFER_SIZE;

  seq_nr_value++;
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
    printf("Init\n");

  case STATE_REGISTERED:
    if(uip_ds6_get_global(ADDR_PREFERRED) != NULL) {
      printf("Registered. Connect attempt %u\n", connect_attempt);
      connect_to_broker();
    }
    else {
      leds_on(LEDS_GREEN);
      ctimer_set(&ct, NO_NET_LED_DURATION, publish_led_off, NULL);
    }
    etimer_set(&publish_periodic_timer, NET_CONNECT_PERIODIC);
    return;
    break;

  case STATE_CONNECTING:
    leds_on(LEDS_GREEN);
    ctimer_set(&ct, CONNECTING_LED_DURATION, publish_led_off, NULL);
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
        leds_on(LEDS_GREEN);
        ctimer_set(&ct, PUBLISH_LED_ON_DURATION, publish_led_off, NULL);
        publish_temperature();
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
    leds_on(LEDS_GREEN);
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
  printf("MQTT TUNSLIP Publisher Started\n");

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