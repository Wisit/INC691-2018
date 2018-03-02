#include "os.h"
#include "at.h"
#include "wifi.h"

const char *SSID = "ECC-Mobile"; //!! SSID
const char *PASS = "ecclab1122"; //!! Password



//!! ESP8266/UART2 Line Received Callback
void EspLineReceived(void *evt)
{
    //!! Get received line
    const char *line = (const char *)((uart_event_t *)evt)->data.buffer; 

    //!! Process received line for AT
    AT_ProcessLine(line);

    //!! Process received line for WiFi
    WiFi_ProcessLine(line);

    //!! Print received line
    Uart1_AsyncWriteString("recv: ");
    Uart1_AsyncWriteString(line);

}


//!! This callback will be called when ESP not response to the last command
void AtTimeoutCallback(void *evt) {
    //!! AT command timeout!
    Uart1_AsyncWriteString("\r\n\r\n");
    Uart1_AsyncWriteString("\r\n*****************************");
    Uart1_AsyncWriteString("\r\n*    AT COMMAND TIMEOUT!    *");
    Uart1_AsyncWriteString("\r\n*****************************");
}


//!!-------------------------------------------------------
//!! WiFi/Network
//!!-------------------------------------------------------

void WiFiConnectedCallback(void *evt) 
{
    //!! Do somthing when ip address is received
    Uart1_AsyncWriteString("Got IP Address!!\r\n");

    //!! Beep and notification, the module is connected to network
    Beep(100);

    
    //!! Check IP address
    //!! After this command is applied, the wifi.ip and wifi.mac will be updated
    AT_PutCommand("AT+CIFSR\r\n");
}

void WiFiDisconnectCallback(void *evt)
{
    //!! WiFi is disconnected
    Uart1_AsyncWriteString("\r\n\r\n");
    Uart1_AsyncWriteString("\r\n*****************************");
    Uart1_AsyncWriteString("\r\n*      WiFi DISCOMMAND!     *");
    Uart1_AsyncWriteString("\r\n*****************************");
}

int main(void)
{
    //!! Initialize OS
    OS_Init();

    //!! Initialze AT
    AT_CommandInit(AtTimeoutCallback);


    //!! Initialize WiFi
    WiFi_Init(SSID, PASS, WiFiConnectedCallback, WiFiDisconnectCallback);


    //!! Start the main loop for 100 mS, see in addon/esp8266/at.c
    OS_TimerCreate("AT_Loop", 100, 1, AT_Loop);

    //!! WiFi loop
    OS_TimerCreate("WiFi_Loop", 100, 1, WiFi_Loop);

    //!! Print empty lines
    Uart1_AsyncWriteString("\r\n\r\nex14_esp8266_fifi\r\n");
    Beep(20);

    //!! Register LineReceived callback of UART2, the ESP8266
    OS_Uart2SetLineReceivedCallback(EspLineReceived);

    //!! Initialize ESP8266 module
    AT_PutCommand("ATE0\r\n");          //!! Disable echo (Doesn’t send back received command)
    AT_PutCommand("AT+CWMODE=1\r\n");   //!! WIFI mode, 1 = Station mode (client)


    //!! Connect to network
    //!! After this command is applied, the message 
    //!! "WIFI CONNECT" and "WIFI GOT IP" should be received
    char buff[48];
    sprintf(buff, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS);
    AT_PutCommand(buff);

    //!! Start server
    // AT_PutCommand("AT+CIPMUX=1\r\n");
    // AT_PutCommand("AT+CIPSERVER=1,80\r\n");
    // AT_PutCommand("AT+CIFSR\r\n");

    OS_Start();
}
