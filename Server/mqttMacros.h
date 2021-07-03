#include "mqtt.h"
#include "net/rpl/rpl.h"
#include "net/ipv6/sicslowpan.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"

#include <string.h>

PROCESS(MQTTServerProcess, "MQTT Process");

#define MQTT_BRIDGE_IP_ADDR  	  "aaaa::1" // Tunslip VM IP
#define BUFFER_SIZE               64
#define APP_BUFFER_SIZE           512
#define MAX_SLAVE_COUNT			  64

#define DEFAULT_PUBLISH_TOPIC     "mote/send"
#define DEFAULT_SUBSCRIBE_TOPIC   "mote/receive"


#define CONFIG_MODE				   "config/mode"
#define CONFIG_ID_TOPIC 		   "config/ids"

#define DEFAULT_KEEP_ALIVE_TIMER     60

/* Maximum TCP segment size for outgoing segments of our socket */
#define MAX_TCP_SEGMENT_SIZE       32

/*
 * MQTT broker address
 */
static const char *broker_ip = MQTT_BRIDGE_IP_ADDR;
/*---------------------------------------------------------------------------*/
/*
 * A timeout used when waiting for something to happen (e.g. to connect or to
 * disconnect)
 */
#define STATE_MACHINE_PERIODIC     (CLOCK_SECOND >> 1)
/*---------------------------------------------------------------------------*/
/* Provide visible feedback via LEDS during various states */
/* When connecting to broker */
#define CONNECTING_LED_DURATION    (CLOCK_SECOND >> 2)

/* Each time we try to publish */
#define PUBLISH_LED_ON_DURATION    (CLOCK_SECOND)
/*---------------------------------------------------------------------------*/
/* Connections and reconnections */
#define RETRY_FOREVER              0xFF
#define RECONNECT_INTERVAL         (CLOCK_SECOND * 2)

/*
 * Number of times to try reconnecting to the broker.
 * Can be a limited number (e.g. 3, 10 etc) or can be set to RETRY_FOREVER
 */
#define RECONNECT_ATTEMPTS         RETRY_FOREVER
#define CONNECTION_STABLE_TIME     (CLOCK_SECOND * 5)
/*---------------------------------------------------------------------------*/
static struct timer connection_life;
static uint8_t connect_attempt;
/*---------------------------------------------------------------------------*/
/* Various states */
static uint8_t state;

#define STATE_INIT                    0
#define STATE_REGISTERED              1
#define STATE_CONNECTING              2
#define STATE_CONNECTED               3
#define STATE_PUBLISHING              4
#define STATE_DISCONNECTED            5
#define STATE_NEWCONFIG               6
#define STATE_CONFIG_ERROR         0xFE
#define STATE_ERROR                0xFF
/*---------------------------------------------------------------------------*/
#define CONFIG_IP_ADDR_STR_LEN       64
/*---------------------------------------------------------------------------*/
/* A timeout used when waiting to connect to a network */
#define NET_CONNECT_PERIODIC        (CLOCK_SECOND >> 2)
#define NO_NET_LED_DURATION         (NET_CONNECT_PERIODIC >> 1)
/**
 * \brief Data structure declaration for the MQTT client configuration
 */
typedef struct mqtt_client_config {
  char broker_ip[CONFIG_IP_ADDR_STR_LEN];  
  clock_time_t pub_interval;
  uint16_t broker_port;
} mqtt_client_config_t;
