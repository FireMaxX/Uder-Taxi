/* Response.h*/
/* Header file for Response.c*/
/* Decode the commands string receivd from the socket, response correspondingly and give responsive string back to the socket*/
/* Arthor: Yan Zhang*/
/* 3/16/2017*/

#ifndef RESPONSE_H
#define RESPONSE_H
#include "map.h"
#include "utils.h"
#include "ServerManagement.h"

int match(const char *string,char *pattern);

char* Response(char* req, Map* map, Taxi* Taxis[],DriverOrder *DriverOrders[]);

#endif
