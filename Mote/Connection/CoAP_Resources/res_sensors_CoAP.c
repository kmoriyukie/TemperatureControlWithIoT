/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      res_sensors.c
 * \author
 *      Lucyanno Frota and Milena Mori
 *  
 *   Based on example code by Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include "contiki.h"

#include <string.h>
#include "contiki.h"
#include "rest-engine.h"
#include "../../master.h"
#include "stdbool.h"

#include "msg.h"
#include "../../states.h"

extern ROLE_t node_role;
extern MODE_t node_mode;


void readJSON_sensor(const char *json, int *params_u, float *params_f);

static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_sensors,
         "title=\"Sensor\"",
         NULL,
         res_post_handler,
         NULL,
         NULL);


static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	if((node_role != MASTER) || (node_mode != WORKING)) return;


	static uint8_t *incoming = NULL;
	static uint8_t size = 0;
	size = REST.get_request_payload(request,(const uint8_t **)&incoming);


	static char json[70];
	memcpy(json,incoming,size);

	static int params_u[3];
	static float params_f[3];

	readJSON_sensor(json, params_u, params_f);

	static struct slave_msg_t msg;

	msg.local_id = params_u[0];
	msg.remote_id = params_u[1];
	msg.temperature = params_f[0];
	msg.humidity = params_f[1];
	msg.airflow = params_f[2];
	msg.battery = params_u[2];

	if(push_packet(&msg)) REST.set_response_payload(response, MSG_SUCCESS, 16);
	else REST.set_response_payload(response, MSG_FAILURE, 16);

}

// MSG_SUCCESS
// MSG_FAILURE
// MSG_ERROR_INVALID_PARAMETERS
// MSG_MOTE_ALREADY_EXISTS
// MSG_MOTE_NOT_FOUND
