
#include "os.h"
#include "server.h"

//!!--------------------------------------------------------------------------------------------
//!! Network
const char      *SSID       = "ECC-Mobile";                 //!! SSID
const char      *PASS       = "ecclab1122";              //!! PASS

//!!--------------------------------------------------------------------------------------------
//!! SoftAP
const char      *AP_SSID    = "ECC-Lab";                //!! SoftAP/Host SSID
const char      *AP_PASS    = "embedded";               //!! 8-character or longer are required
const char      *AP_IPADDR  = "192.168.0.1";            //!! IP address
const uint8_t   AP_CHANNEL  = 2;                        //!! 2, 3, 4, 5 work (0, 1 not work)
const uint8_t   AP_ENCRYPT  = WIFI_AP_ENCRYPT_WPA_PSK;  //!! Encryption type

//!!--------------------------------------------------------------------------------------------

//!! Note: In SoftAP, using external files, e.g.; jQuery, Bootstrap and others
//!!       are not posible. However. those files can be loaded from the cache of the browser
//!!       To make the files saved in the cache, just enter IP address of the server 
//!!       (IP address of Station, not SoftAP) 
//!!--------------------------------------------------------------------------------------------



#include "page.h"

//!!--------------------------------------------------------------



//!! AT command state changed callback function
void AT_Callback(void *evt)
{
    // at_command_t *at = (at_command_t *)evt;
    // char buff[32];
    // sprintf(buff, ">>AT: state:%d return:%d\r\n", at->state, at->returnCode);
    // Uart1_AsyncWriteString(buff);
}

//!! WiFi state changed callback function
void WiFi_Callback(void *evt)
{
    // char buff[32];
    // wifi_t *wifi = (wifi_t *)evt;
    // sprintf( buff, ">>WiFi: state:%d\r\n", wifi->state );
    // Uart1_AsyncWriteString(buff);
}

//!!
//!!#define _ESP_LINE_RECEIVED_DEBUG_
//!!

//!! UART2/ESP8266 LineReceived callback function
void ESP_LineReceived(void *evt) 
{
    const char *line = (const char *)((uart_event_t *)evt)->data.buffer;
    
    #ifdef _ESP_LINE_RECEIVED_DEBUG_
        //!! Debug
        Uart1_AsyncWriteString( "recv: " );
        Uart1_AsyncWriteString( line );
    #endif //!! _ESP_LINE_RECEIVED_DEBUG_

    //!! Services
    AT_Service( (void *)line );
    ESP_Service( (void *)line );
    WiFi_Service( (void *)line );
    Server_Service( (void *)line );
}


//!! Main loop, the callback function of a 20 mS Timer
void MainLoop(void *evt) 
{
    //!! Services
    AT_Service( NULL );
    ESP_Service( NULL );
    WiFi_Service( NULL );
    Server_Service( NULL );

    //!! Toggle LED0
    LED0_Inv();
}

//!! Server state changed and Client requested callback function
void Client_Requested(void *evt)
{
    //!! Take the server
    server_t * server = (server_t *)evt;
    client_t * client = server->client;

    //!! Is it come with a client?
    if(server->client != NULL)
    {
        //!! homepage (index.html or empty)
        if( !strcmp(client->getBuffer, "") || !strcmp(client->getBuffer, "index.html")  )
        {
            client->data = homeHtml;
        }
        //!! Get the app.css
        else if( !strcmp(client->getBuffer, "app.css") ) 
        {
            client->data = appCss;
        }
        //!! Get the app.js
        else if( !strcmp(client->getBuffer, "app.js") ) 
        {
            client->data = appJs;
        }
        //!!Get the favicon.ico
        else if( !strcmp(client->getBuffer, "favicon.ico") )
        {
            client->data = faviconIco;
        }
        //!!------------------------------------------------------------------------------
        //!!
        //!! Check the get command
        //!! The response message can be storaged in heap memory or flash (constant string)
        //!!
        else if (!strcmp(server->client->getBuffer, "adc1"))
        {
            //!! Allocate memory for storing response data
            //!! This memory will be freed after all bytes are sent
            char *buff = malloc(64);


            //!! Check the allocated address
            //!! Note: A debuging message ">>Free" will be printed internally
            char temp[32];
            sprintf(temp, ">>Heap: 0x%.4X\r\n", (uint16_t)buff);
            Uart1_AsyncWriteString(temp);

            //!! Create response message
            sprintf(buff, "<html><h1>ADC0: %3.3f Volts</h1></html>", 3.3 *ADC1_Get() / 1023.0);
            server->client->data = buff;//"HELLO WORLD!";

        }
        else if (!strcmp(server->client->getBuffer, "led3-inv"))
        {
            //!! Toggle the LED3
            LED3_Inv();

            //!! Check the LED3 status and construct a respnse message (constant string)
            if( LED3_Get() == LED_ON )
            {
                server->client->data = "<html><h1 style='color:red;'>The LED3 is ON</h1></html>";
            }
            else
            {
                server->client->data = "<html><h1 style='color:gray;'>The LED3 is OFF</h1></html>";    
            }
        }
    }
}

//!! Main function, the entry point
int main(void) 
{
    //!! Initial the OS
    OS_Init();                                 

    //!! Initialise the AT
    AT_Init(AT_Callback);

    //!! Initialise the WiFi module
    ESP_Init(ESP_LineReceived);

    //!! Initialise the WiFi, see above for provided parameters
    WiFi_InitStation( SSID, PASS, WIFI_MODE_AP_STATION );
    WiFi_InitSoftAP( AP_SSID, AP_PASS, AP_IPADDR, AP_CHANNEL, AP_ENCRYPT );

    //!! Start the WiFi operations
    WiFi_Start( WiFi_Callback );

    //!! Start the HTTP server
    Server_Start(Client_Requested);

    //!! Start the main loop timer
    OS_TimerCreate("main", 20, 1, MainLoop);

    //!! Start OS
    OS_Start();                                
}
