
#ifndef __SERVER_H__
#define __SERVER_H__

#include "os.h"

#define SERVER_STATE_STOPPED        0
#define SERVER_STATE_STARTED        1
#define SERVER_STATE_GET_REQUEST    2
#define SERVER_STATE_MAKE_RESPONSE  4
#define SERVER_STATE_END_CONNECTION 5
typedef struct
{
    uint8_t state;
    uint8_t connectionChannel;
    char getCommand[32];
    os_callback_t stateChangedCallback;
} server_t;

void Server_Init(os_callback_t stateChangedCallback);

void Server_Loop(void *evt);

uint16_t Server_ProcessLine(const char *line);

#endif //!! __SERVER_H__