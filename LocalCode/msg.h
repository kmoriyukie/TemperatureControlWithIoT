#ifndef MSG_
#define MSG_
/*---------------------------------------------------------------------------*/
/* This is the UDP port used to send and receive data */
#define UDP_CLIENT_PORT   8765
#define UDP_SERVER_PORT   5678

#define TX_POWER  -16

#define CHANNEL   26
#define PANID     0xDEEC

/*---------------------------------------------------------------------------*/
/* This data structure is used to store the packet content (payload) */

struct sensor_data{
	float temperature;
	float humidity;
	float airflow;
	float battery;
};

struct slave_msg_t{
	uint8_t  local_id;
	uint8_t  remote_id;
	struct sensor_data;
};

/*---------------------------------------------------------------------------*/
#endif
