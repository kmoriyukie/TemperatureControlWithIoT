#ifndef MQTTCOM_H_
#define MQTTCOM_H_

#define DEFAULT_PUBLISH_TOPIC     "mote/send"
#define DEFAULT_SUBSCRIBE_TOPIC   "mote/receive"


#define CONFIG_MOTE_CLOUDMODE_TOPIC	   "config/mote/cloudmode"
#define CONFIG_CLOUD_CLOUDMODE_TOPIC	   "config/cloud/cloudmode"


#define CONFIG_MOTE_ID_TOPIC 		   "config/mote/ids"
#define CONFIG_CLOUD_ID_TOPIC 		   "config/cloud/ids"

void mqttcom_pub(char *topic, char *msg);

// void mqttcom_rec(char *msg,uint16_t len);

#endif