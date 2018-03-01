#include "os.h"
#include "at.h"

//!! Comment this line to disable received line echo
#define __PRINT_RECEIVED_MESSAGE__

//!! Comment this line to disable beep sound when timeout occured
#define __BEEP_WHEN_TIMEOUT__


//!! ESP8266/UART2 Line Received Callback
void EspLineReceived(void *evt)
{
    //!! Get received line
    const char *line = (const char *)((uart_event_t *)evt)->data.buffer; 

    //!! Process received line
    AT_ProcessLine(line);

    #ifdef __PRINT_RECEIVED_MESSAGE__
    //!! Print received line
    Uart1_AsyncWriteString("recv: ");
    Uart1_AsyncWriteString(line);
    #endif //!! __PRINT_RECEIVED_MESSAGE__
}


//!! This callback will be called when ESP not response to the last command
void EspTimeoutCallback(void *evt) {

    #ifdef __BEEP_WHEN_TIMEOUT__
    //!! Do somthing when timeout!
    Beep_FreqSet(200); Beep(50);
    #endif //!!__BEEP_WHEN_TIMEOUT__
}

int main(void)
{
    //!! Initialize OS
    OS_Init();

    //!! Initialze AT
    AT_CommandInit(EspTimeoutCallback);

    //!! Start the main loop for 100 mS, see in addon/esp8266/at.c
    OS_TimerCreate("AT_Loop", 100, 1, AT_Loop);
    
    //!! Print empty lines
    Uart1_AsyncWriteString("\r\n\r\n");

    //!! Register LineReceived callback of UART2, the ESP8266
    OS_Uart2SetLineReceivedCallback(EspLineReceived);

    //!! Initialize ESP8266 module
    AT_PutCommand("ATE0\r\n");          //!! Disable echo (Doesn’t send back received command)
    AT_PutCommand("AT+CWMODE=1\r\n");   //!! WIFI mode, 1 = Station mode (client)

    //!! Other command can be applied here

    //!! Start OS
    OS_Start();
}
