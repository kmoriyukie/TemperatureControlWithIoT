#include "stdlib.h"
#include "string.h"
#include "stdint.h"

void readJSON_i(const char *json, int *params){
    const static char sep1[] = ":";
    const static char sep2[] = ",";
    const static char sep3[] = "}";
    char *ptr1 = NULL;
    char *ptr2 = NULL;
    char buff[16];
    int i = 0;
    while(i == 0 || (ptr1 != NULL && ptr2 != NULL)){
        if(i == 0){
            ptr1 = strstr(json,sep1);
            ptr2 = strstr(json,sep2);
        }
        else{
            ptr1 = strstr(ptr1+1,sep1);
            ptr2 = strstr(ptr2+1,sep2);
        }
        if(ptr1 != NULL && ptr2 == NULL) ptr2 = strstr(json,sep3);
        memset(buff,0,16);
        int j = 2;
        for(j = 2; j < (ptr2-ptr1); j++){
            buff[j-2] = ptr1[j];
        }
        params[i] = atoi(buff);
        i++;
    }
}


void readJSON_f(const char *json, float *params){
    const static char sep1[] = ":";
    const static char sep2[] = ",";
    const static char sep3[] = "}";
    char *ptr1 = NULL;
    char *ptr2 = NULL;
    char buff[16];
    int i = 0;
    while(i == 0 || (ptr1 != NULL && ptr2 != NULL)){
        if(i == 0){
            ptr1 = strstr(json,sep1);
            ptr2 = strstr(json,sep2);
        }
        else{
            ptr1 = strstr(ptr1+1,sep1);
            ptr2 = strstr(ptr2+1,sep2);
        }
        if(ptr1 != NULL && ptr2 == NULL) ptr2 = strstr(json,sep3);
        memset(buff,0,16);
        int j = 2;
        for(j = 2; j < (ptr2-ptr1); j++){
            buff[j-2] = ptr1[j];
        }
        params[i] = atof(buff);
        i++;
    }
}

void readJSON_uf(const char *json, uint8_t *params_u, float *params_f){
    const static char sep1[] = ":";
    const static char sep2[] = ",";
    const static char sep3[] = "}";
    char *ptr1 = NULL;
    char *ptr2 = NULL;
    char buff[16];
    uint8_t i = 0; uint8_t f = 0;
    while((i+f) == 0 || (ptr1 != NULL && ptr2 != NULL)){
        if((i+f) == 0){
            ptr1 = strstr(json,sep1);
            ptr2 = strstr(json,sep2);
        }
        else{
            ptr1 = strstr(ptr1+1,sep1);
            ptr2 = strstr(ptr2+1,sep2);
        }
        if(ptr1 != NULL && ptr2 == NULL) ptr2 = strstr(json,sep3);
        memset(buff,0,16);
        int j = 2;
        for(j = 2; j < (ptr2-ptr1); j++){
            buff[j-2] = ptr1[j];
        }
        switch((i+f)){
            case 0: case 1: case 5:
                params_u[i] = atoi(buff);        
                i++;
            break;
            case 2: case 3: case 4:
                params_f[f] = atof(buff);
                f++;
            break;
        }
    }
}