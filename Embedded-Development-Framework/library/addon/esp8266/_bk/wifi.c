#include "at.h"
#include "wifi.h"

const char *WiFiStateMsg[] = {
    "[0] WIFI_STATE_DISCONNECTED",
    "[1] WIFI_STATE_CONNECTED",
    "[2] WIFI_STATE_GOT_IP_ADDR",
    "[3] WIFI_STATE_IP_MAC_UPDATED",
    "[4] WIFI_STATE_NETWORK_FAILED",
    "[5] WIFI_STATE_NETWORK_TIMEOUT"};

//!! Global object
wifi_t wifi;

//!!
void WiFi_Init(const char *SSID, const char *PASS, os_callback_t WiFiStateChangedCallback) {
    wifi.ssid = (char *)SSID;
    wifi.pass = (char *)PASS;
    wifi.stateChangedCallback = WiFiStateChangedCallback;
    wifi.state = WIFI_STATE_DISCONNECTED;
    wifi.ip[0]  = 0; // Null terminate, make it to empty buffer
    wifi.mac[0] = 0; // Null terminate, make it to empty buffer
}


//!!
//!! Helper functions/Macros
//!!

//!! Helper: Clear wifi.ip and wifi.mac by applying the terminator (0 or NULL)
#define WiFi_ClearIpMac() { \
    wifi.ip[0]  = 0;        \
    wifi.mac[0] = 0;        \
}

//!! Helper: Change state and execute callback
#define WiFi_ApplyStateChange(newState) {           \
    wifi.state = newState;                          \
    if(wifi.state == WIFI_STATE_DISCONNECTED ||     \
        wifi.state == WIFI_STATE_NETWORK_FAILED) {  \
        WiFi_ClearIpMac();                          \
    }                                               \
    if(wifi.stateChangedCallback != NULL) {         \
        wifi.stateChangedCallback((void *)&wifi);   \
    }                                               \
}

//!! Helper: Apply software RESET command to the module
#define WiFi_ResetModule() {                \
    wifi.state   = WIFI_STATE_DISCONNECTED; \
    WiFi_ClearIpMac();                      \
    /* Be sure a new command can be sent */ \
    at.state = AT_STATE_READY_FOR_COMMAND;  \
    AT_PutCommand("AT+RST\r\n");            \
}


//!! This callback function must be called by LineReceived callbck of the UART2/ESP8266
uint16_t WiFi_ProcessLine(const char *line) {

    //!! WIFI_STATE_DISCONNECTED
    if (wifi.state == WIFI_STATE_DISCONNECTED)
    {
        if (!strcmp(line, "WIFI CONNECTED\r\n"))
        {
            //!! Changed from DISCONNECTED to CONNECTED
            WiFi_ApplyStateChange(WIFI_STATE_CONNECTED);
        }
        if (!strcmp(line, "FAIL\r\n"))
        {
            //!! Changed from DISCONNECTED to FAILED
            WiFi_ApplyStateChange(WIFI_STATE_NETWORK_FAILED);

            //!! In this example, when the connection failed, clear all state
            //!! parameters and reset the module. Don't fotget to change the at.state to
            //!! AT_STATE_WAIT_COMMAND to make the command can be sent to the module
            WiFi_ResetModule();
        }
        
    }
    //!! WIFI_STATE_CONNECTED
    else if (wifi.state == WIFI_STATE_CONNECTED)
    {
        if (!strcmp(line, "WIFI DISCONNECT\r\n"))
        {
            //!! Changed from CONNECTED to DISCONNECTED
            WiFi_ApplyStateChange(WIFI_STATE_DISCONNECTED);
        }
        else if (!strcmp(line, "WIFI GOT IP\r\n"))
        {
            //!! Changed from CONNECTED to GOT_IP_ADDR
            WiFi_ApplyStateChange(WIFI_STATE_GOT_IP_ADDR);

            //!! IP address is received, make a request for ip and mac addresses
            //!! After this command is sent, the module will return CIFSR:STAIP and +CIFSR:STAMAC
            //!! Check code below, how to receive the ip and mac addresses and 
            //!! fill them into the wifi.ip and wifi.mac
            AT_PutCommand("AT+CIFSR\r\n");
        }
    }
    //!! WIFI_STATE_GOT_IP_ADDR
    else if (wifi.state == WIFI_STATE_GOT_IP_ADDR)
    {
        if (!strcmp(line, "WIFI DISCONNECT\r\n"))
        {   
            //!! Changed from WIFI_STATE_GOT_IP_ADDR to WIFI_STATE_DISCONNECTED
            WiFi_ApplyStateChange(WIFI_STATE_DISCONNECTED);
        }
    }
    //!! WIFI_STATE_IP_MAC_UPDATED
    else if (wifi.state == WIFI_STATE_IP_MAC_UPDATED)
    {
        if (!strcmp(line, "WIFI DISCONNECT\r\n"))
        { 
            //!! Changed from WIFI_STATE_IP_MAC_UPDATED to WIFI_STATE_DISCONNECTED
            WiFi_ApplyStateChange(WIFI_STATE_DISCONNECTED);
        }
    }
    //!!
    //!! The command "AT+CIFSR\r\n" is applied, 
    //!! the module will return the +CIFSR:STAIP and +CIFSR:STAMAC
    if (wifi.state != WIFI_STATE_IP_MAC_UPDATED)
    {
        if (str_index_of_first_token(line, "+CIFSR:STAIP,") == 0 ||
            str_index_of_first_token(line, "+CIFSR:STAMAC,") == 0)
        {
            //!! IP Format: +CIFSR:STAIP,"192.168.43.2"
            uint8_t i1 = str_index_of_first_char(line, '"');
            uint8_t i2 = str_index_of_last_char(line, '"');
            uint8_t nb = i2 - i1 - 1;

            //!! IP Address
            if (line[10] == 'I')  {
                memcpy(wifi.ip, line + (i1 + 1), nb);
                wifi.ip[nb] = 0; //!! Null terminate
            }
            //!! MAC Address
            else if (line[10] == 'M')  {
                memcpy(wifi.mac, line + (i1 + 1), nb);
                wifi.mac[nb] = 0; //!! Null terminate
            }

            //!! Both IP and MAC addresses are updated
            if (strlen(wifi.ip) > 0 && strlen(wifi.mac) > 0)  {
                WiFi_ApplyStateChange(WIFI_STATE_IP_MAC_UPDATED);
            }
        }
    }

    return wifi.state;
}

//!! Must be called periodically, e.g.; every 100 mS
void WiFi_Loop(void *evt) {
    static uint8_t prev_state;
    static uint8_t prev_sec = 0;
    static uint16_t waitingSec = 0;
    os_time_t time = OS_TimeGet();
    if (time.ss != prev_sec)
    {
        //!! Save the current seconds
        prev_sec = time.ss;

        if (wifi.state == WIFI_STATE_DISCONNECTED || wifi.state == WIFI_STATE_NETWORK_FAILED)
        {
            waitingSec++;

            //!! Cannot connect to network for a long time, e.g.; 10 seconds, 
            if (waitingSec >= 10)
            {
                waitingSec = 0;

                //!! Save the current state
                prev_state = wifi.state;

                //!! Appy state changed, WIFI_STATE_DISCONNECTED and WIFI_STATE_NETWORK_FAILED
                //!! can generate timeout event
                WiFi_ApplyStateChange(WIFI_STATE_NETWORK_TIMEOUT);

                //!! Restore the previous state
                wifi.state = prev_state;

                //!! Make hanshake if needed!
                //AT_PutCommand("AT\r\n");
            }
        }
        else{
            //!! Reset the counter
            waitingSec = 0;
        }

        // char buff[48];
        // sprintf(buff, "%.2d:%.2d:%.2d ", time.hh, time.mm, time.ss);
        // Uart1_AsyncWriteString(buff);
        // sprintf(buff, "wifi.state: %s [%d]\r\n", WiFiStateMsg[wifi.state], waitingSec);
        // Uart1_AsyncWriteString(buff);
    }
    
}
