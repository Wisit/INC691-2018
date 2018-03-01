#ifndef __WIFI_H__
#define __WIFI_H__

#include "os.h"

typedef struct
{
    uint8_t state;
    char *ssid;
    char *pass;
    char ip[16];  // 192.168.123.456
    char mac[18]; // 5c:cf:7f:23:e1:08
    os_callback_t connectedCallback;
    os_callback_t disconnectedCallback;
} wifi_t;

extern const char *WiFiStateMsg[];
extern wifi_t wifi;

void WiFi_Init(const char *SSID, const char *PASS, 
    os_callback_t WiFiConnectedCallback, os_callback_t WiFiDisconnectCallback);
void WiFi_ProcessLine(const char *line);
#endif
