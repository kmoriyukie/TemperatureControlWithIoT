#ifndef MSG_
#define MSG_
/*---------------------------------------------------------------------------*/
/* This is the UDP port used to send and receive data */
/*#define UDP_CLIENT_PORT   8765
#define UDP_SERVER_PORT   5678

#define TX_POWER  -16

#define CHANNEL   26
*/
/*---------------------------------------------------------------------------*/
/* This data structure is used to store the packet content (payload) */

#include "stdint.h"
// #include "stdbool.h"

struct sensor_data{
	float temperature; // degrees celsius
	float humidity; // xxx%
	float airflow; // cm/s
	uint8_t battery; // xxx%
};

struct slave_msg_t{
	struct slave_msg_t *next;
	uint8_t local_id;
	uint8_t remote_id;
	float temperature; // degrees celsius
	float humidity; // xxx%
	float airflow; // cm/s
	uint8_t battery; // xxx%
};

struct MOTE_t{
	struct MOTE_t *next;
	uint8_t local_id;
	uint8_t remote_id;
};

// struct master_msg_t{
	
// };
#define MSG_SUCCESS "{\"Response\": 1}\n" //16
#define MSG_FAILURE "{\"Response\": 0}\n" //16
#define MSG_ERROR_INVALID_PARAMETERS "{\"Response\": -1}\n" //17
#define MSG_MOTE_ALREADY_EXISTS "{\"Response\": -2}\n" //17
#define MSG_MOTE_NOT_FOUND "{\"Response\": -3}\n" //17
#define MSG_CONFIG_MODE "{\"Response\": -9}\n" //17


/*---------------------------------------------------------------------------*/
#endif
