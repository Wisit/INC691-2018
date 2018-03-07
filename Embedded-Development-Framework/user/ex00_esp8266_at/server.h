
#ifndef __SERVER_H__
#define __SERVER_H__

#include "at.h"

typedef void (*server_callback_t)(void *);

typedef struct 
{   

    const char *ssid;
    const char *pass;
    char ip[16];        //!! xxx.xxx.xxx.xxx
    char mac[18];       //!! 5c:cf:7f:23:e1:08 
    server_callback_t callback;
}server_t;

void Server_ExecuteLine(const char *line);
void HTTP_ServerInit(const char *ssid, const char *pass, server_callback_t callback);

#endif