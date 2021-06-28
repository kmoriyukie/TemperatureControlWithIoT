#ifndef SLAVE_H
#define SLAVE_H

#define N_READINGS 8
#define SR_READINGS 3

#include "msg.h"

// void reset_sensor_data(struct sensor_data *sensor);

// void Passign_sensor_data(struct sensor_data *sensor,float temp,float hum,float air,float bat);

// void filter(void);

// void mean_sensor_data(struct sensor_data *sensor,uint8_t n);

uint16_t read_temperature(void);

uint16_t read_humidity(void);

uint16_t read_airflow(void);

uint16_t read_battery(void);

void take_reading(struct sensor_data *sensor);

void event_readings(void);

#endif