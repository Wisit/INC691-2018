
#include "os.h"
#include "server.h"



void ESPLineReceived(void *evt) {
    //!! Get received line
    const char *line = (const char *)((uart_event_t *)evt)->data.buffer;
    //Uart1_AsyncWriteString("recv: ");
    //Uart1_AsyncWriteString(line);

    Server_ExecuteLine(line);
    AT_ExecuteLine(line);

    LED_Flash(LED_ID_0, 20);

}


int main(void) 
{
     const char *SSID = "WLRT02";
        const char *PASS = "WLRT11112";

    //!! Initial the OS
    OS_Init();
    AT_Init();
    HTTP_ServerInit(SSID, PASS, NULL);
   
    OS_Uart2SetLineReceivedCallback(ESPLineReceived);
   
    //!! Setup commands
    AT_Put("AT+CIPSERVER=0\r\n");
    AT_Put("AT+RST\r\n");
    AT_Put("ATE0\r\n");
    AT_Put("AT+CIPMUX=1\r\n"); 
    AT_Put("AT+CWMODE=1\r\n");  

       
    char buff[48];
    sprintf(buff, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS);
    AT_Put(buff);
   


    //!! Start OS
    OS_Start();
}


