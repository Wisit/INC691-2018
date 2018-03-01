#include "wifi.h"

#define WIFI_STATE_DISCONNECTED 0
#define WIFI_STATE_CONNECTED    1
#define WIFI_STATE_GOT_IP_ADDR  2

const char *WiFiStateMsg[] = {
    "WIFI_STATE_DISCONNECTED",
    "WIFI_STATE_CONNECTED",
    "WIFI_STATE_GOT_IP_ADDR"};

wifi_t wifi;

void WiFi_Init(const char *SSID, const char *PASS, 
    os_callback_t WiFiConnectedCallback, os_callback_t WiFiDisconnectCallback)
{
    wifi.ssid = (char *)SSID;
    wifi.pass = (char *)PASS;
    wifi.connectedCallback = WiFiConnectedCallback;
    wifi.disconnectedCallback = WiFiDisconnectCallback;
    wifi.state = WIFI_STATE_DISCONNECTED;
    wifi.ip[0]  = 0; // Null terminate, make it to empy buffer
    wifi.mac[0] = 0; // Null terminate, make it to empy buffer
}

void WiFi_ProcessLine(const char *line) {

    //!! WIFI_STATE_DISCONNECTED
    if (wifi.state == WIFI_STATE_DISCONNECTED)
    {
        if (!strcmp(line, "WIFI CONNECTED\r\n"))
        {
            wifi.state = WIFI_STATE_CONNECTED;
        }
    }
    //!! WIFI_STATE_CONNECTED
    else if (wifi.state == WIFI_STATE_CONNECTED)
    {
        if (!strcmp(line, "WIFI DISCONNECT\r\n"))
        {
            wifi.state = WIFI_STATE_DISCONNECTED;
            if (wifi.disconnectedCallback != NULL)
            {
                wifi.disconnectedCallback((void *)&wifi);
            }
        }
        else if (!strcmp(line, "WIFI GOT IP\r\n"))
        {
            wifi.state = WIFI_STATE_GOT_IP_ADDR;
            if (wifi.connectedCallback != NULL)
            {
                wifi.connectedCallback((void *)&wifi);
            }
        }
    }
    //!! WIFI_STATE_GOT_IP_ADDR
    else if (wifi.state == WIFI_STATE_GOT_IP_ADDR)
    {
        if (!strcmp(line, "WIFI DISCONNECT\r\n"))
        {
            wifi.state = WIFI_STATE_DISCONNECTED;
            if (wifi.disconnectedCallback != NULL)
            {
                wifi.disconnectedCallback((void *)&wifi);
            }
        }
    }

    //-----------------------------------------------------------------------
    if (str_index_of_first_token(line, "+CIFSR:STAIP,") == 0)
    { //!! IP Format: +CIFSR:STAIP,"192.168.43.2"
        uint8_t i1 = str_index_of_first_char(line, '"');
        uint8_t i2 = str_index_of_last_char(line, '"');
        uint8_t nb = i2 - i1 - 1;
        memcpy(wifi.ip, line + (i1 + 1), nb);
        wifi.ip[nb] = 0;    //!! Null terminate
    }
    else if (str_index_of_first_token(line, "+CIFSR:STAMAC,") == 0)
    {
        //!! MAC Format: +CIFSR:STAMAC,"5c:cf:7f:23:e1:08"
        uint8_t i1 = str_index_of_first_char(line, '"');
        uint8_t i2 = str_index_of_last_char(line, '"');
        uint8_t nb = i2 - i1 - 1;
        memcpy(wifi.mac, line + (i1 + 1), nb);
        wifi.mac[nb] = 0; //!! Null terminate
    }
}
