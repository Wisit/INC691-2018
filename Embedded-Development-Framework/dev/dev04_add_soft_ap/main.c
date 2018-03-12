
#include "os.h"
#include "at2.h"
#include "wifi2.h"
#include "server2.h"

//!!--------------------------------------------------------------------------------------------
const char *SSID    =   "SSID";
const char *PASS    =   "PASS";

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


void ClientRequested(void *evt)
{
    client_t *client = ((server_t *)evt)->client;

    if( !strcmp( client->getBuffer, "led0-inv") ) {
        LED0_Toggle();
        char *pPage = malloc(128);
        const char *status = LED0_Get() ? "ON" : "OFF";
        const char *color  = LED0_Get() ? "red" : "gray";
        sprintf(pPage, "<html><body><h2 style='color:%s;'>LED0: %s </h2></body></html>", color, status);
        client->data = pPage;
    }
    if( !strcmp( client->getBuffer, "adc1") ) {
        char *pPage = malloc(128);
        sprintf(pPage, "<html><body><h2>ADC1: %1.3f Volts</h2></body></html>", ADC1_Get()*3.3/1023.0);
        client->data = pPage;
    }
    Beep(50);
}

int main(void) 
{

    OS_Init();      

    LED_ModeSet(LED_ID_3, LED_MODE_PWM);
    LED_PwmSet(LED_ID_3, 100, 0, 20);

    Uart1_WriteString("\r\nHTTP Server Application!\r\n"); 
    Server_Init(SSID, PASS, 3, ClientRequested);
    WiFi_InitSoftAP( AP_SSID, AP_PASS, AP_IPADDR, AP_CHANNEL, AP_ENCRYPT );


    OS_Start();                                
}
