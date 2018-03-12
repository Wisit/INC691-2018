
#include "os.h"
#include "at2.h"
#include "wifi2.h"
#include "server2.h"


const char *SSID    =   "WLRT02";
const char *PASS    =   "WLRT11112";



void ClientRequested(void *evt)
{
    client_t *client = ((server_t *)evt)->client;

    if( !strcmp( client->getBuffer, "led0-inv") ) {
        LED0_Toggle();
        char *pPage = malloc(128);
        const char *status = LED0_Get() == LED_ON ? "ON" : "OFF";
        const char *color  = LED0_Get() == LED_ON ? "red" : "gray";
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
    OS_Start();                                
}
