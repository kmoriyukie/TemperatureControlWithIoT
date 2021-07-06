#include "readJSON.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

static float my_pow(int v,int p);
static float my_atof(char *);

static float my_pow(int v,int p){
    int i;
    int ret = 1;
    for(i = 0; i < p; i++){
        if(i == 0) ret = v;
        else ret*=v;
    }
    return (float) ret;
}

static float my_atof(char *c){
    const static char sep[] = ".";
    char *ptr1 = NULL;
    char *ptr2 = NULL;
    char buff[17];
    float aux;
    
    if(strlen(c) < 3) return ((float) atoi(c));
    ptr1 = c;
    ptr2 = strstr(c,sep);
    int j = 0;
    for(j = 0; j < (ptr2-ptr1); j++){
        buff[j] = ptr1[j];
    }
    buff[j] = '\0';
    aux = atoi(buff)+atoi(&(ptr2[1]))/my_pow(10,strlen(&(ptr2[1])));
    
    return aux;
}

// extern float strtof( const char* str, char** str_end );

void readJSON_uf(const char *json, int *params_u, float *params_f){
    const static char sep1[] = ":";
    const static char sep2[] = ",";
    const static char sep3[] = "}";

    // printf("Size rj: %u, %u\n", sizeof(params_u),sizeof(*params_u));
    char *ptr1 = NULL;
    char *ptr2 = NULL;
    // char *ptr3 = NULL;
    char buff[16];
    int i = 0; int f = 0;
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
                params_f[f] = my_atof(buff);
                f++;
            break;
        }
    }
}

void readJSON_i(const char *json, int *params_i){
    const static char sep1[] = ":";
    const static char sep2[] = ",";
    const static char sep3[] = "}";

    // printf("Size rj: %u, %u\n", sizeof(params_i),sizeof(*params_u));
    char *ptr1 = NULL;
    char *ptr2 = NULL;
    // char *ptr3 = NULL;
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
        params_i[i] = atoi(buff);
        i++;
        // switch((i+f)){
        //     case 0: case 1: case 5:
        //         params_u[i] = atoi(buff);        
        //         i++;
        //     break;
        //     case 2: case 3: case 4:
        //         params_f[f] = my_atof(buff);
        //         f++;
        //     break;
        // }
        // if((i+f) >= len) break;
    }
}