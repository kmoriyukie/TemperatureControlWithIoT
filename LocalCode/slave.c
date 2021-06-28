#include "slave.h"

#include "contiki.h"

#if CONTIKI_TARGET_ZOUL
// #include "dev/adc-zoul.h"
#include "dev/zoul-sensors.h"
#else /* Assumes the Z1 mote */
// #include "dev/tmp102.h"
// #include "dev/adxl345.h"
#include "sens_temperature.h"
#include "sens_humidity.h"
#include "sens_airflow.h"
#include "sens_battery.h"

#endif

#include "stdint.h"
#include "stdio.h"

PROCESS(take_readings, "Readings");

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
	static struct sensor_data temp_readings;

	while(1){

		uint32_t temperature = 0;
		uint32_t humidity = 0;
		uint32_t airflow = 0;
		uint16_t battery = 0;

	for(i = 0; i < N_READINGS; i++){
		temperature += read_temperature();
		humidity += read_humidity();
		airflow += read_airflow();
		battery += read_battery();

	}

	temp_readings.temperature = (float) (temperature>>SR_READINGS);
	temp_readings.humidity = (float) ((humidity>>SR_READINGS)*HUMIDITY_CONST);
	temp_readings.airflow = (float) (airflow>>SR_READINGS);
	temp_readings.battery = (uint8_t) ((battery>>SR_READINGS)*BATTERY_CONST);


	printf("Temperature: %i\n",(int)temp_readings.temperature);
	printf("Humidity: %u%%\n",(int)temp_readings.humidity);
	printf("Airflow: %u\n",(int)temp_readings.airflow);
	printf("Battery: %u%%\n",temp_readings.battery);
	}
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
		return PERCENT_CONST*(batt-2750);
	#else
		static int batt;
		batt = battery_sensor.value(0);
		if(batt < 2.75*1000) return (uint8_t) 0;
		else return (uint16_t) (battery_sensor.value(0)-2.75*1000);
		return 0;
	#endif
}

void take_reading(struct sensor_data *sensor){
	// static uint32_t batt;
	// batt = battery_sensor.value(1);
	// batt *= 5000;
 //    batt >>= 12;
 //    printf("Batt: %u\n", batt);
	// Passign_sensor_data(sensor,0,0,0,(uint16_t)batt);
}

void event_readings(void){
	process_start(&take_readings,"");
}