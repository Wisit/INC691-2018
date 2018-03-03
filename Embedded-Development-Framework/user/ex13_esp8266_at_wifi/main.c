#include "os.h"
#include "at.h"
#include "wifi.h"
#include "server.h"

const char *SSID = "ECC-Mobile"; //!! SSID
const char *PASS = "ecclab1122"; //!! Password




//#define _PRINT_RECEIVED_LINE_
//#define _PRINT_FIFI_STATE_CHANGED_
//#define _PRINT_AT_STATE_CHANGED_

//!! Debug: Print WiFi state changed (will be move to wifi.c later)
#ifdef _PRINT_FIFI_STATE_CHANGED_
void WiFi_PrintStateChanged(const char *msg) {
    char buff[32];
    os_time_t time = OS_TimeGet();
    sprintf(buff, "\r\nTime: %.2d:%.2d:%.2d\r\n", time.hh, time.mm, time.ss);
    Uart1_AsyncWriteString("\r\n");
    Uart1_AsyncWriteString("\r\n********************************");
    Uart1_AsyncWriteString(buff);
    Uart1_AsyncWriteString(msg);
    Uart1_AsyncWriteString("\r\n********************************");
    Uart1_AsyncWriteString("\r\n");
}
#else
void WiFi_PrintStateChanged(const char *msg)
{
    (void)msg;
}
#endif

//!! Debug: Print IP and MAC addresses (will be move to wifi.c later)
void WiFi_PrintIpMac(wifi_t *p) {
    Uart1_AsyncWriteString("\r\n");
    Uart1_AsyncWriteString("\r\n********************************");
    Uart1_AsyncWriteString("\r\nNetwork Info:");
    Uart1_AsyncWriteString("\r\n -IP Address:  ");
    Uart1_AsyncWriteString(p->ip);
    Uart1_AsyncWriteString("\r\n -MAC Address: ");
    Uart1_AsyncWriteString(p->mac);
    Uart1_AsyncWriteString("\r\n********************************");
    Uart1_AsyncWriteString("\r\n");
}

//!! User's callback
//!! Callback function, called by OS when a line is received 
void WiFiStageChangedCallback( void *evt ) {

    //!! The wifi_t object is passed to this callback function, cast it to wifi_t
    wifi_t * p = (wifi_t *)evt;

    //!! Print state changed information
    WiFi_PrintStateChanged(WiFiStateMsg[p->state]);
    
    //!! Put your code in the "case" below
    switch(p->state) 
    {
        //!!--- [0] ---------------------------------------------------
        case WIFI_STATE_DISCONNECTED:
        {
            //!!
            //!! Disconnected from network    
            //!!
            // UserFunction();
        }
        break;

        //!!--- [1] ---------------------------------------------------
        case WIFI_STATE_CONNECTED:
        {
            //!!
            //!! Connected to network
            //!!
            // UserFunction();
        }
        break;

        //!!--- [2] ---------------------------------------------------
        case WIFI_STATE_GOT_IP_ADDR:
        {
            //!!
            //!! Received IP address but the wifi.ip and wifi.mac are not updated yet
            //!!
            // UserFunction();
        }
        break;

        //!!--- [3] ---------------------------------------------------
        case WIFI_STATE_IP_MAC_UPDATED:
        {
            //!!
            //!! Received IP address, both wifi.ip and wifi.mac are fully updateed
            //!!
            // UserFunction();

            //!! Print IP and MAC addresses
            WiFi_PrintIpMac(p);
        }
        break;

        //!!--- [4] ---------------------------------------------------
        case WIFI_STATE_NETWORK_FAILED:
        {
            //!!
            //!! Cannot connected to network, check SSID, PASS, and other network parameters
            //!!
            // UserFunction();
        }
        break;
       
        //!!--- [5] ---------------------------------------------------
        case WIFI_STATE_NETWORK_TIMEOUT:
        {
            //!!
            //!! Cannot connected to network for a long time, e.g.; 10 seconds
            //!!
            // UserFunction();
        }
        break;
    }
}


//!! Debug: Print AT state changed (will be moved to at.c later)
#ifdef _PRINT_AT_STATE_CHANGED_
void AT_PrintStateChanged(const char *msg)
{
    char buff[32];
    os_time_t time = OS_TimeGet();
    sprintf(buff, "\r\nTime: %.2d:%.2d:%.2d\r\n", time.hh, time.mm, time.ss);
    Uart1_AsyncWriteString("\r\n");
    Uart1_AsyncWriteString("\r\n********************************");
    Uart1_AsyncWriteString(buff);
    Uart1_AsyncWriteString(msg);
    Uart1_AsyncWriteString("\r\n********************************");
    Uart1_AsyncWriteString("\r\n");
}
#else
void AT_PrintStateChanged(const char *msg) {
    (void) msg;
}
#endif


//!! When the AT command's state is changed, this callback function will be called
void ATStateChangedCallback(void *evt) {

    //!! A at object is passed form internal state machines, case it to at_command_t
    at_command_t *p = (at_command_t *)evt;

    //!! Print internal state of the AT's state machines
    AT_PrintStateChanged(AtStateMessage[p->state]);

    switch(p->state) {

        //!!--- [0] ---------------------------------------------------
        case AT_STATE_MODULE_DISCONNECTED:
        {
            //!!
            //!! No WiFi module is detected
            //!!
            // UserFunction();
        }
        break;

        //!!--- [1] ---------------------------------------------------
        case AT_STATE_MODULE_CONNECTEED:
        {
            //!!
            //!! The WiFi module is detected, it ready to receive some commands
            //!!
            // UserFunction();
        }
        break;

        //!!--- [2] ---------------------------------------------------
        case AT_STATE_READY_FOR_COMMAND:
        {
            //!!
            //!! Ready for a new command
            //!!
            // UserFunction();
        }
        break;

        //!!--- [3] ---------------------------------------------------
        case AT_STATE_WAIT_OK_RESPONSE:
        {
            //!!
            //!! A command is sent to the module, waiting for a respose message
            //!!
            // UserFunction();
        }
        break;

        //!!--- [4] ---------------------------------------------------
        case AT_STATE_GOT_OK_RESPONSE:
        {
            //!!
            //!! The module successfully exetuctes the command
            //!!
            // UserFunction();
        }
        break;

        //!!--- [5] ---------------------------------------------------
        case AT_STATE_WAIT_TIMEOUT:
        {
            //!!
            //!! A command is sent for a long time, e.g.; 10 seconds,
            //!! but no positive response is returned
            //!!
            // UserFunction();
            Beep(1000);
        }
        break;
    }
}

//!! ESP8266/UART2 Line Received Callback
void EspLineReceived(void *evt)
{
    //!! Get received line
    const char *line = (const char *)((uart_event_t *)evt)->data.buffer;

    #ifdef _PRINT_RECEIVED_LINE_
    //!! Print only non empty line
    if (strcmp(line, "\r\n"))
    {
        Uart1_AsyncWriteString("recv: ");
        Uart1_AsyncWriteString(line);
    }
    #endif

    //!! Process received line for AT
    AT_ProcessLine(line);

    //!! Process received line for WiFi
    WiFi_ProcessLine(line);

    //!!
    Server_ProcessLine(line);
}

//!! Callback function of OS looper
void Main_Looper(void *evt)
{
    auto uint16_t ms = OS_TimeMilliseconds();
    if (ms % 50 == 0)
    {
        AT_Loop((void *)0);
        WiFi_Loop((void *)0);
        Server_Loop((void *)0);
    }
}

//!! Main
int main(void)
{
    //!! Initialize OS
    OS_Init();

    //!! Print empty lines and information
    Uart1_AsyncWriteString("\r\n\r\nIoT Firmware Initial Version (v0.0)\r\n");
    LED_Flash(LED_ID_0, 100);       // Flash LED0 for 0.1 sec
    Beep_FreqSet(500); Beep(50);    // 500 Hz Beep sound for 50 mS


    //!! Initialze AT
    AT_CommandInit(ATStateChangedCallback);


    //!! Initialize WiFi
    WiFi_Init(SSID, PASS, WiFiStageChangedCallback);

    Server_Init(NULL);

    
    //!!
    //!! Note: The AT and WiFi can share the timer for their main loop
    //!!       To do so, call the AT_Loop and WiFi_Loop in the same callback of a timer or looper
    OS_LooperCreate("looper", Main_Looper);

    //!! Register LineReceived callback of UART2, the ESP8266
    OS_Uart2SetLineReceivedCallback(EspLineReceived);

    //!! Initialize ESP8266 module
    AT_PutCommand("ATE0\r\n");          //!! Disable echo (Doesn’t send back received command)
    AT_PutCommand("AT+CWMODE=1\r\n");   //!! WIFI mode, 1 = Station mode (client)

    //!!
    //!! Connect to network
    //!!
    //!! After this command is applied, the message 
    //!! "WIFI CONNECT" and "WIFI GOT IP" should be received
    char buff[48];
    sprintf(buff, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS);
    AT_PutCommand(buff);

    //!!
    //!! Start server
    //!!
    AT_PutCommand("AT+CIPMUX=1\r\n");
    AT_PutCommand("AT+CIPSERVER=1,80\r\n");

    //!!
    //!! To test this application, do the following steps:
    //!!   1) Check IP address of the SERVER
    //||   2) Open the browser and give it the IP address
    //!!   3) Check the requested message in the console
    //!!   4) The requested message will be processed later

    //!!
    //!! TO DO: Implement the HTTP server!
    //!!
    
    //!! Start the OS
    OS_Start();
}
