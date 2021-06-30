#include "slave.h"

#include "contiki.h"

#if CONTIKI_TARGET_ZOUL
#include "dev/zoul-sensors.h"
#else /* Assumes the Z1 mote */
#include "sens_temperature.h"
#include "sens_humidity.h"
#include "sens_airflow.h"
#include "sens_battery.h"
#endif

#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"

static struct sensor_data readings;

bool reading_done_flag = false;

PROCESS(slave_working, "Slave Workinig");
PROCESS(take_readings, "Readings");

PROCESS_THREAD(slave_working, ev, data){
	PROCESS_BEGIN();

	static struct etimer et;
	static struct etimer et_timeout;

	etimer_set(&et, READINGS_TIME*CLOCK_SECOND);
	etimer_set(&et_timeout, TIMOUT*CLOCK_SECOND);

	bool timeout_flag = false;
	static int Ntry;

	Ntry = 0;

	printf("Slave Workinig\n");

	while(1){

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		reading_done_flag = false;

		process_start(&take_readings,"");

		etimer_reset(&et_timeout);

		while(reading_done_flag == false){
			PROCESS_YIELD();
			if(ev = etimer_expired(&et_timeout)){
				printf("TIMEOUT\n");
				Ntry++;
				if(Ntry >= MTRY){
					timeout_flag = true;
					break;
				}
				else{
					process_start(&take_readings,"");
					etimer_reset(&et_timeout);
				}
			}
		}

		if(!timeout_flag){
			printf("Temperature: %i, Humidity: %i, AriFlow: %i, Battery: %u\n", (int)(readings.temperature),
																				(int)(readings.humidity),
																				(int)(readings.airflow),
																				readings.battery);

			etimer_reset(&et);
		}
	}

	PROCESS_END();
}

bool sensors_initialized = false;

PROCESS_THREAD(take_readings, ev, data){
	PROCESS_BEGIN();

	if(!sensors_initialized){
		#if CONTIKI_TARGET_ZOUL

		#else
		sens_temperature_initialize();
		sens_humidity_initialize();
		sens_airflow_initialize();
		sens_battery_initialize();
		#endif
	}

	static uint8_t i;

	static uint32_t temperature;
	static uint32_t humidity;
	static uint32_t airflow;
	static uint16_t battery;

	temperature = 0;
	humidity = 0;
	airflow = 0;
	battery = 0;

	for(i = 0; i < N_READINGS; i++){
		#if CONTIKI_TARGET_ZOUL
		#else
			temperature += read_temperature();
			humidity += read_humidity();
			airflow += read_airflow();
			battery += read_battery();
		#endif
	}

	#if CONTIKI_TARGET_ZOUL
	#else
		readings.temperature = (float) (temperature>>SR_READINGS);
		readings.humidity = (float) ((humidity>>SR_READINGS)*HUMIDITY_CONST);
		readings.airflow = (float) (airflow>>SR_READINGS);
		readings.battery = (uint8_t) ((battery>>SR_READINGS)*BATTERY_CONST);
	#endif


	// printf("Temperature: %i\n",(int)readings.temperature);
	// printf("Humidity: %u%%\n",(int)readings.humidity);
	// printf("Airflow: %u\n",(int)readings.airflow);
	// printf("Battery: %u%%\n",readings.battery);
	reading_done_flag = true;
	PROCESS_END();
}

// void event_readings(){
// 	process_start(&take_readings,"");
// }

void exec_slave_working(void){
	process_start(&slave_working,"");
}

void exit_slave_working(void){
	if(process_is_running(&take_readings)!=0){
		process_exit(&take_readings);	
	}
	process_exit(&slave_working);
}

void exec_slave_config(void){

}

void exit_slave_config(void){

}