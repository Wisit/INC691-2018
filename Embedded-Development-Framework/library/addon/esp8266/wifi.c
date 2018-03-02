#include "at.h"
#include "wifi.h"

#define WIFI_STATE_DISCONNECTED     0
#define WIFI_STATE_CONNECTED        1
#define WIFI_STATE_GOT_IP_ADDR      2
#define WIFI_STATE_IP_UPDATED       3   //!![3] IP and MAC addresses are filled in the wifi.ip and wifi.mac
#define WIFI_STATE_FAILED           4
const char *WiFiStateMsg[] = {
    "[0] WIFI_STATE_DISCONNECTED",
    "[1] WIFI_STATE_CONNECTED   ",
    "[2] WIFI_STATE_GOT_IP_ADDR ",
    "[3] WIFI_STATE_IP_UPDATED  ",
    "[4] WIFI_STATE_FAILED      "};

wifi_t wifi;

void WiFi_Init(const char *SSID, const char *PASS, 
    os_callback_t WiFiConnectedCallback, os_callback_t WiFiDisconnectCallback)
{
    wifi.ssid = (char *)SSID;
    wifi.pass = (char *)PASS;
    wifi.connectedCallback    = WiFiConnectedCallback;
    wifi.disconnectedCallback = WiFiDisconnectCallback;
    wifi.stateChangedCallback = NULL;
    wifi.state = WIFI_STATE_DISCONNECTED;
    wifi.ip[0]  = 0; // Null terminate, make it to empy buffer
    wifi.mac[0] = 0; // Null terminate, make it to empy buffer
}

#define WiFi_ApplyStateChange(newState) {           \
    wifi.state = newState;                          \
    if(wifi.stateChangedCallback != NULL) {         \
        wifi.stateChangedCallback((void *)&wifi);   \
    }                                               \
}

void WiFi_ProcessLine(const char *line) {

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
            WiFi_ApplyStateChange(WIFI_STATE_FAILED);

            //!! Reconnected or Reset is required
            Beep(200);
            Uart1_AsyncWriteString("Failed, reseting...\r\n");

            //!! The atComd.state is needed to be changed to AT_STATE_WAIT_COMMAND
            //!! to allow the new command can be sent to the ESP
            atComd.state = AT_STATE_WAIT_COMMAND;  
            AT_PutCommand("AT+RST\r\n");
        }
        
    }
    //!! WIFI_STATE_CONNECTED
    else if (wifi.state == WIFI_STATE_CONNECTED)
    {
        if (!strcmp(line, "WIFI DISCONNECT\r\n"))
        {
            //!! Changed from CONNECTED to DISCONNECTED
            WiFi_ApplyStateChange(WIFI_STATE_DISCONNECTED);
            if (wifi.disconnectedCallback != NULL)
            {
                wifi.disconnectedCallback((void *)&wifi);
            }
        }
        else if (!strcmp(line, "WIFI GOT IP\r\n"))
        {
            //!! Changed from CONNECTED to GOT_IP_ADDR
            WiFi_ApplyStateChange(WIFI_STATE_GOT_IP_ADDR);
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
        {   //!! Changed from WIFI_STATE_GOT_IP_ADDR to WIFI_STATE_DISCONNECTED
            WiFi_ApplyStateChange(WIFI_STATE_DISCONNECTED);
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

void WiFi_Loop(void *evt) {
    static uint8_t prev_sec = 0;
    static uint16_t waitingSec = 0;
    os_time_t time = OS_TimeGet();
    if (time.ss != prev_sec)
    {
        if (wifi.state == WIFI_STATE_DISCONNECTED) {
            waitingSec++;
            if (waitingSec >= 10)
            {
                waitingSec = 0;
                Beep(200);
                AT_PutCommand("AT+CIFSR\r\n");
            }
        }
        else{
            waitingSec = 0;
        }

        char buff[48];
        sprintf(buff, "%.2d:%.2d:%.2d ", time.hh, time.mm, time.ss);
        Uart1_AsyncWriteString(buff);
        sprintf(buff, "wifi.state: %s [%d]\r\n", WiFiStateMsg[wifi.state], waitingSec);
        Uart1_AsyncWriteString(buff);
    }
    prev_sec = time.ss;
}
