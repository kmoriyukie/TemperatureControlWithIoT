#ifndef READ_JSON_H_
#define READ_JSON_H_

#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

static float my_pow(int v,int p);
static float my_atof(char *);
void readJSON_uf(const char *json, int *params_u, float *params_f);

#endif
