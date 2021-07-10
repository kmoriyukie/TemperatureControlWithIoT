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
 *      res_config.c
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
#include "stdlib.h"

#include "msg.h"
#include "../../states.h"

extern ROLE_t node_role;
extern MODE_t node_mode;

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);


void readJSON_i(const char *json, int *params_i);

RESOURCE(res_blink,
         "title=\"Blink?ID=XX\"",
         res_get_handler,
         NULL,
         NULL,
         NULL);

bool find_onBlink_list(uint8_t ID, struct blink_t **item);

void blinkList_remove(struct blink_t **item);

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset){
	static uint8_t size = 0;

	const char *par = NULL;

	static uint8_t ID = 0;

	size = REST.get_query_variable(request, "ID", &par);
	if(size > 3 || size < 1){
		REST.set_response_payload(response, MSG_ERROR_INVALID_PARAMETERS, 17);
		return;
	}
	else{
		ID = atoi(par);
		static	struct blink_t *aux = NULL;
		if(find_onBlink_list(ID, &aux)){
			blinkList_remove(&aux);
			REST.set_response_payload(response, MSG_SUCCESS, 16);
		}
		else REST.set_response_payload(response, MSG_FAILURE, 16);
	}
}

// MSG_SUCCESS
// MSG_FAILURE
// MSG_ERROR_INVALID_PARAMETERS
// MSG_MOTE_ALREADY_EXISTS
// MSG_MOTE_NOT_FOUND
