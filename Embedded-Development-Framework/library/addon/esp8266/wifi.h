#ifndef __WIFI_H__
#define __WIFI_H__

#include "os.h"

#define WIFI_STATE_DISCONNECTED     0   //!! [0] Disconnected from network
#define WIFI_STATE_CONNECTED        1   //!! [1] Connected to network
#define WIFI_STATE_GOT_IP_ADDR      2   //!! [2] Received IP address
#define WIFI_STATE_IP_MAC_UPDATED   3   //!! [3] IP and MAC addresses are filled in the wifi.ip and wifi.mac
#define WIFI_STATE_NETWORK_FAILED   4   //!! [4] Cannot make connection to network, check SSID, PASS and others
#define WIFI_STATE_NETWORK_TIMEOUT  5   //!! [5] Cannot connect to network or network failed for T seconds

typedef struct
{
    uint8_t state;                      //!! WiFi state
    char *ssid;                         //!! SSID
    char *pass;                         //!! PASS
    char ip[16];                        //!! IP address, e.g.; 192.168.123.456
    char mac[18];                       //!! MAC address e.g.; 5c:cf:7f:23:e1:08
    os_callback_t stateChangedCallback;
} wifi_t;

//!! Make them visible to other files
extern const char *WiFiStateMsg[];
extern wifi_t wifi;

//!! Initial all parameters of the wifi object and assign state changed callback function
//!! This function must be called in startup
void WiFi_Init(const char *SSID, const char *PASS, os_callback_t WiFiStateChangedCallback);

//!! Process all received lines
//!! This function must be called from the LineReceived callback of the ESP/UART2
uint16_t WiFi_ProcessLine(const char *line);

//!! WiFi main loop processing
//!! This function is a callback function of timer
//!! Don't forget to creat a timer and give this function as its callback
void WiFi_Loop(void *evt);

#endif //!! __WIFI_H__
//!! EOF
