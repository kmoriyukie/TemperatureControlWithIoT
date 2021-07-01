// /*
//  * Copyright (c) 2010, Swedish Institute of Computer Science.
//  * All rights reserved.
//  *
//  * Redistribution and use in source and binary forms, with or without
//  * modification, are permitted provided that the following conditions
//  * are met:
//  * 1. Redistributions of source code must retain the above copyright
//  *    notice, this list of conditions and the following disclaimer.
//  * 2. Redistributions in binary form must reproduce the above copyright
//  *    notice, this list of conditions and the following disclaimer in the
//  *    documentation and/or other materials provided with the distribution.
//  * 3. Neither the name of the Institute nor the names of its contributors
//  *    may be used /*
//  * Copyright (c) 2010, Swedish Institute of Computer Science.
//  * All rights reserved.
//  *
//  * Redistribution and use in source and binary forms, with or without
//  * modification, are permitted provided that the following conditions
//  * are met:
//  * 1. Redistributions of source code must retain the above copyright
//  *    notice, this list of conditions and the following disclaimer.
//  * 2. Redistributions in binary form must reproduce the above copyright
//  *    notice, this list of conditions and the following disclaimer in the
//  *    documentation and/or other materials provided with the distribution.
//  * 3. Neither the name of the Institute nor the names of its contributors
//  *    may be used to endorse or promote products derived from this software
//  *    without specific prior written permission.
//  *
//  * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
//  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
//  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//  * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//  * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//  * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//  * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//  * SUCH DAMAGE.
//  *
//  */


//  * \file
//  *         sens_battery header file.
//  * \author
//  *	   Modifications made by Lucyanno Frota
//  *      
//  *         Based on code of:
//  *         Adam Dunkels <adam@sics.se>
//  *         Joakim Eriksson <joakime@sics.se>
//  *         Niclas Finne <nfi@sics.se>
 

#include "sens_battery.h"

#if CONTIKI_TARGET_ZOUL
 
#else 

	#include "dev/sky-sensors.h"

	#define INPUT_CHANNEL      (1 << INCH_11)
	#define INPUT_REFERENCE    SREF_1
	#define BATTERY_MEM    ADC12MEM11

	const struct sensors_sensor battery_sensor;

	/*---------------------------------------------------------------------------*/
	static int
	value(int type)
	{
	  return BATTERY_MEM;
	}
	/*---------------------------------------------------------------------------*/
	static int
	configure(int type, int c)
	{
	  return sky_sensors_configure(INPUT_CHANNEL, INPUT_REFERENCE, type, c);
	}
	/*---------------------------------------------------------------------------*/
	static int
	status(int type)
	{
	  return sky_sensors_status(INPUT_CHANNEL, type);
	}
	/*---------------------------------------------------------------------------*/
	SENSORS_SENSOR(battery_sensor, BATTERY_SENSOR,
	               value, configure, status);

	void sens_battery_initialize(void){
		SENSORS_ACTIVATE(battery_sensor);
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

#endif