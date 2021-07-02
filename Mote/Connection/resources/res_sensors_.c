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
 *      config.c
 * \author
 *      Lucyanno Frota
 *  
 *   Based on example code by Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include "contiki.h"

#include <string.h>
#include "contiki.h"
#include "rest-engine.h"
#include "master.h"
#include "stdbool.h"
#include "stdlib.h"

 #include "msg.h"


static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_sensors,
         "title=\"Config\"",
         NULL,
         res_post_handler,
         NULL,
         NULL);


static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){

	// Verificaçao do modo


	static uint8_t *incoming = NULL;
	static uint8_t size = 0;
	size = REST.get_request_payload(request,(const uint8_t **)&incoming);


	// if(size > 11 || size < 9){
	// 	REST.set_response_payload(response, MSG_ERROR_INVALID_PARAMETERS, 16);
	// 	return;
	// }



	//Verificaçao de tamanho


	static char json[70];
	memcpy(json,incoming,size);

	static uint8_t params_u[3];
	static float params_f[3];
	readJSON_uf(json, params_u, params_f);

	static struct slave_msg_t msg;

	msg.local_id = params_u[0];
	msg.remote_id = params_u[1];
	msg.temperature = params_f[0];
	msg.humidity = params_f[1];
	msg.airflow = params_f[2];
	msg.battery = params_u[2];

	// printf("St:\n L_ID: %u, R_ID: %u, Temp: %i, Humi: %i, Air: %i, Bat: %u\n",params_u[0],params_u[1],(uint8_t)(params_f[0]*10),(uint8_t)(params_f[1]*10),(uint8_t)(params_f[2]*10),params_u[2]);

	if(push_packet(&msg)) REST.set_response_payload(response, MSG_SUCCESS, 15);
	else REST.set_response_payload(response, MSG_FAILURE, 15);

}

// MSG_SUCCESS
// MSG_FAILURE
// MSG_ERROR_INVALID_PARAMETERS
// MSG_MOTE_ALREADY_EXISTS
// MSG_MOTE_NOT_FOUND
