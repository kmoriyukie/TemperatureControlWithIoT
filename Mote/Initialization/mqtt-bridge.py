# This "bridge" script subscribes to messages inside the 6LoWPAN network using unencrypted MQTT
# and then publishes the messages securely to the MQTT broker in the AWS cloud using TLS/SSL

# You can easily change this script, in case you need to subscribe to topics from the AWS cloud 
# and publish them locally in the 6LoWPAN network.

# Libraries:
import paho.mqtt.client as mqtt
import ssl
import sys


############ Please change these parameters for your own setup #########


# There is a Mosquitto server running locally on the Virtual Machine. 
# For the project, you should change this to the IPv6 server of the local MQTT broker:
LOCAL_MQTT_URL = "localhost"
# You may also try "test.mosquitto.org"

# A topic to get information from the zolertia board(s):	 
# SENSORS_TOPIC = "mote/send"

# Address of the Cloud MQTT Broker:
CLOUD_MQTT_URL = "a2aqpcbaxix62g-ats.iot.eu-west-1.amazonaws.com"
CERTIFICATE_AUTH_FILE = "/home/user/GitHub/TemperatureControlWithIoT/Mote/Initialization/iot_certs/AmazonRootCA1.pem"
CERT_PEM_FILE = "/home/user/GitHub/TemperatureControlWithIoT/Mote/Initialization/iot_certs/9b94e0ec55-certificate.pem.crt"
PRIVATE_KEY_FILE = "/home/user/GitHub/TemperatureControlWithIoT/Mote/Initialization/iot_certs/9b94e0ec55-private.pem.key"

# A topic to get information from the cloud:	 
CONFIG_MOTE_CLOUDMODE_TOPIC = "config/mote/cloudmode"
CONFIG_CLOUD_CLOUDMODE_TOPIC = "config/cloud/cloudmode"

CONFIG_MOTE_ID_TOPIC = "config/mote/ids"
CONFIG_CLOUD_ID_TOPIC = "config/cloud/ids"

#########################################################################

# Callback for initial local network connection:
def on_connect(local_client, userdata, flags, rc):
    print("Connected to local MQTT broker with result code " + str(rc))
    local_client.subscribe(CONFIG_MOTE_CLOUDMODE_TOPIC)
    print("Subscribed to local topic: " + CONFIG_MOTE_CLOUDMODE_TOPIC + "\n")

    local_client.subscribe(CONFIG_MOTE_ID_TOPIC)
    print("Subscribed to local topic: " + CONFIG_MOTE_ID_TOPIC + "\n")

# Callback for received message in the local network:
def on_local_message(local_client, userdata, msg):
    #publish the exact same message on the MQTT broker in the cloud:
    print("Local -> Cloud: Topic [" + msg.topic + "]. Msg \""+str(msg.payload)+"\"")
    cloud_client.publish(msg.topic,str(msg.payload))

# Callback for received message in the cloud:
def on_cloud_message(cloud_client, userdata, msg):
    #publish the exact same message on the local MQTT broker:
    print("Cloud -> Local: Topic [" + msg.topic + "]. Msg \""+str(msg.payload)+"\"")
    local_client.publish(msg.topic,str(msg.payload))

#########################################################################

# 1st connect to the Cloud MQTT Client:
cloud_client=mqtt.Client() 
cloud_client.on_message = on_cloud_message

print("Connecting to the Cloud at " + CLOUD_MQTT_URL + "...")
cloud_client.tls_set(CERTIFICATE_AUTH_FILE, CERT_PEM_FILE, PRIVATE_KEY_FILE, tls_version=ssl.PROTOCOL_TLSv1_2)
cloud_client.tls_insecure_set(False)
cloud_client.connect(CLOUD_MQTT_URL, 8883, 60)
print("Connected to the Cloud MQTT Broker.")

cloud_client.subscribe(CONFIG_CLOUD_CLOUDMODE_TOPIC)
print("Subscribed to cloud topic: " + CONFIG_CLOUD_CLOUDMODE_TOPIC + "\n")

cloud_client.subscribe(CONFIG_CLOUD_ID_TOPIC)
print("Subscribed to cloud topic: " + CONFIG_CLOUD_ID_TOPIC + "\n")

# Then conect to the Local MQTT Client:
local_client = mqtt.Client()
local_client.on_connect = on_connect
local_client.on_message = on_local_message

print("Connecting locally to " + LOCAL_MQTT_URL + "...")
local_client.connect(LOCAL_MQTT_URL, 1883, 60)

# If we had just 1 connection we coud use client.loop_forever()
# since we have two, we'll use a while loop this way:

try:
   while True:
        local_client.loop(0.01) # timeout of 0.01 secs (max 100Hz)
        cloud_client.loop(0.01)

except (KeyboardInterrupt): #catch keyboard interrupts
        sys.exit()


# You can test by running on your VM:
# mosquitto_pub -h localhost -t zolertia/sensor_status -m "Publiquei esta msg localmente e a bridge fez forward para a cloud!"

# Also test publishing something in the cloud and check if you receive it locally with:
# mosquitto_sub -h localhost -t cloud/action


