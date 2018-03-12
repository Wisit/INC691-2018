#ifndef __WIFI_H__
#define __WIFI_H__

#include "os.h"


#define WIFI_STATE_DISCONNECTED     0
#define WIFI_STATE_CONNECTED        1
#define WIFI_STATE_GOT_IP           2
#define WIFI_STATE_STARTING_SRV     3
#define WIFI_STATE_SERVER_RUNNING   4

typedef struct 
{
    uint8_t         state;
    os_callback_t   callback;           //!! State changed callback function
    const char      *ssid;              //!! Pointer to SSID
    const char      *pass;              //!! Pointer to PASS
    uint8_t         mode;               //!! Operation mode
    char            ip[16];             //!! xxx.xxx.xxx.xxx
    char            mac[18];            //!! 5c:cf:7f:23:e1:08 
}wifi_t;

void WiFi_Service(void); 
void WiFi_ProcessLine(const char *line); 
void WiFi_Init(const char *ssid, const char *pass, uint8_t mode);

#endif //!! __WIFI_H__
