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
	static struct etimer et_timout;

	etimer_set(&et, READINGS_TIME*CLOCK_SECOND);
	etimer_set(&et_timout, TIMOUT*CLOCK_SECOND);

	bool timout_flag = false;
	static int Ntry;

	Ntry = 0;

	printf("Slave Workinig\n");

	while(1){

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		reading_done_flag = false;

		process_start(&take_readings,"");

		etimer_reset(&et_timout);

		while(reading_done_flag == false){
			PROCESS_YIELD();
			if(ev = etimer_expired(&et_timout)){
				printf("TIMOUT\n");
				Ntry++;
				if(Ntry >= MTRY){
					timout_flag = true;
					break;
				}
				else{
					process_start(&take_readings,"");
					etimer_reset(&et_timout);
				}
			}
		}

		if(!timout_flag){
			printf("Temperature: %i, Humidity: %i, AriFlow: %i, Battery: %u\n", (int)(readings.temperature),
																				(int)(readings.humidity),
																				(int)(readings.airflow),
																				readings.battery);

			etimer_reset(&et);
		}
	}

	PROCESS_END();
}

PROCESS_THREAD(take_readings, ev, data){
	PROCESS_BEGIN();

	#if CONTIKI_TARGET_ZOUL

	#else
	SENSORS_ACTIVATE(temperature_sensor);
	SENSORS_ACTIVATE(humidity_sensor);
	SENSORS_ACTIVATE(airflow_sensor);
	SENSORS_ACTIVATE(battery_sensor);
	#endif


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
		// if(exit_working) break;
		temperature += read_temperature();
		humidity += read_humidity();
		airflow += read_airflow();
		battery += read_battery();

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

uint16_t read_temperature(void){
	#if CONTIKI_TARGET_ZOUL
		return 32;
	#else
		return (uint16_t) (((uint16_t)temperature_sensor.value(0))*TEMPERATURE_CONSTS_1 + TEMPERATURE_CONSTS_0);
	#endif
}

uint16_t read_humidity(void){
	#if CONTIKI_TARGET_ZOUL
		return 60;
	#else
		return (uint16_t) (((uint16_t)humidity_sensor.value(0))*HUMIDITY_CONSTS_1);
	#endif
}

uint16_t read_airflow(void){
	#if CONTIKI_TARGET_ZOUL
		return 110;
	#else
		return (uint16_t) (((uint16_t)airflow_sensor.value(0))*AIRFLOW_CONSTS_1);
	#endif
}

uint16_t read_battery(void){
	#if CONTIKI_TARGET_ZOUL
		static uint32_t batt;
		batt = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
		return (uint16_t) (10.0/145.0)*(batt-2750);
	#else
		static int batt;
		batt = battery_sensor.value(0);
		if(batt < 2.75*1000) return (uint8_t) 0;
		else return (uint16_t) (battery_sensor.value(0)-2.75*1000);
		return 0;
	#endif
}

void event_readings(/*struct sensot_data *sens*/){
	process_start(&take_readings,"");
}

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