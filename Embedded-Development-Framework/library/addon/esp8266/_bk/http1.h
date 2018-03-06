
#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__


#include "config.h"
#include "stimer.h"


//!!
//!! Uncomment this line to debug server loop
//!!
//#define _SERVER_DEBUG_

//!!
//!! Uncomment this line to debug line received
//!!
#define _ESP_LINE_RECEIVED_



typedef void (*server_callback_t)(void *);


#define AT_RETURN_ESP_READY         0//(0x0001<<0)
#define AT_RETURN_OK                1//(0x0001<<1)     //!! OK
#define AT_RETURN_SEND_OK           2//(0x0001<<2)     //!! SEND OK
#define AT_RETURN_NO_CHANGE         3//(0x0001<<3)     //!! Noting changed
#define AT_RETURN_LINK_BUILDED      4//(0x0001<<4)     //!! Link is builded
#define AT_RETURN_CLOSED            5//(0x0001<<5)     //!! The connection is closed
#define AT_RETURN_UNLINK            6//(0x0001<<6)     //!! Unlinked
#define AT_RETURN_BUSY              7//(0x0001<<7)     //!! Not ready
#define AT_RETURN_ERROR             8//(0x0001<<8)     //!! ERROR
#define AT_RETURN_FAIL              9//(0x0001<<9)     //!! FAILED
#define AT_RETURN_SEND_FAIL         10//(0x0001<<10)     //!! SEND FILED
#define AT_RETURN_UNKNOWN           11//(0x0001<<11)    //!! Others

#define AT_STATE_READY              0//(0x0001<<0)
#define AT_STATE_WAIT_RESPONSE      1//(0x0001<<1)
#define AT_STATE_GOT_RESPONSE       2//(0x0001<<2)
#define AT_STATE_TIMEOUT            3//(0x0001<<3)
typedef struct {
    uint16_t state;
    uint16_t returnType;
    uint16_t timeoutTicks;
}at_command_t;




#define CLIENT_STATE_DISCONNECTED   0
#define CLIENT_STATE_REQUESTED      1
typedef struct {
    uint8_t connectionState;
    uint8_t channel;
    char getCommand[32];
}client_t;
#define CLIENT_CONNECTION_MAX       5





#define SERVER_STATE_IDLE           0   //!! No active client
#define SERVER_STATE_BUSY           1   //!! In progress... 

typedef struct
{
    uint8_t   state;

    uint8_t   loopState;
    uint8_t   nextState;
    uint8_t   nextResType;  //!! Next response type

    uint8_t   timeoutTicks;

    client_t  *target;
    client_t  *clients;
    timer_t   *timer; 

    const char *ssid;
    const char *pass;

    char ip[16];        //!! xxx.xxx.xxx.xxx
    char mac[18];       //!! 5c:cf:7f:23:e1:08

    server_callback_t callback;
}server_t;


typedef struct {
    uint8_t state;
    uint8_t nextState;
    uint8_t timeoutTicks;
    timer_t *timer;
}esp_init_t;


void HTTP_ServerInit(void);

#endif
