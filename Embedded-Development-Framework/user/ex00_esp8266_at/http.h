#ifndef __HTTP_H__
#define __HTTP_H__

#include "at.h"
#include "esp.h"
#include "wifi.h"


//!!-------------------------------------------------------

#define NUM_CLIENTS                 5
#define CLIENT_GET_BUFFER_LENGTH    32

#define CLIENT_STATE_DISCONNECTED    0
#define CLIENT_STATE_REQUESTED       1
#define CLIENT_STATE_WAIT_OK         2
#define CLIENT_STATE_WAIT_SEND_OK    3
#define CLIENT_STATE_WAIT_CLOSE_OK   4
typedef struct {
    uint8_t  state;                                 //!! State index
    uint8_t  channel;                               //!! Connected channel [0, 4]
    uint32_t milliSeconds;                          //!! Milliseconds, used for multiclientle frames sending   
    uint16_t restFrames;                            //!! Number of the rest frames needed to be sent
    uint16_t restBytes;                             //!! Number of the rest bytes needed to be sent
    char     getBuffer[CLIENT_GET_BUFFER_LENGTH];   //!! Get buffer, 32 bytes maximum
    const char *data;                               //!! Pointer to data needed to be sent
}client_t;

//!!-------------------------------------------------------

typedef void (*server_callback_t)(void *);

typedef struct 
{   
    timer_t    *timer;
    client_t   *client;
    wifi_t     *wifi;
    server_callback_t callback;
}server_t;

//!!-------------------------------------------------------
// void HTTP_ServerInit( server_callback_t callback );
// void HTTP_Service( void *evt );


void Server_Service(void *evt) 


#endif //!! __HTTP_H__