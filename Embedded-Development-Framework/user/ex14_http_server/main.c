/***************************************************
 *               Dr.Santi Nuratch                  *
 *    Embedded Computing and Control Laboratory    *
 *               27 Fubruary, 2018                 *
 * *************************************************
 *      ESP8266 WiFi module, AT command mode       *
 *   This example shows how to create HTTP Server  *
 *   The program is implemented using even-driven  *
 *  and callback technique. Each function operates *
 *              as Asynchronous Operation          *
 ***************************************************
 */


#include "os.h" //!! ECC-OS and all utillities functions

//!!*********************************
//!! Network required parameters
//!!*********************************
const char *SSID = "ECC-Mobile"; // SSID
const char *PASS = "ecclab1122"; // Password

//!!*********************************
//!! System Indicators
//!!*********************************
//!! LED3 flashing 5 Hz: cannot connect to the network
//!!                     or no IP address is provided
//!! LED3 flashing 1 Hz: connected to network and got an IP address
//!! Low Frequency Beep: command timeout, the WiFi module not response

//!!*********************************
//!! Setting
//!!*********************************
//!! Uncomment this line to check failed state of the WiFi module
//!! The failed state will be occured when the WiFi module cannot
//!! detect the SSID or the provided PASS is not corrected

//#define CHECK_WIFI_STATE_FAILED

os_callback_t HttpGetHandler = NULL;

//!!*********************************
//!! WiFi/Network structure
//!!*********************************
typedef struct
{
    int state;
    const char *ssid;
    const char *pass;
} wifi_t;
wifi_t wifi;
#define WIFI_STATE_DISCONNECTED 0 // WiFi/Network disconnected
#define WIFI_STATE_CONNECTED 1    // WiFiNetwork connected and got IP
#define WIFI_STATE_FAILED 2       //!! AT+RST or Reconnecting is resqired

//!!*********************************
//!! AT Command structure
//!!*********************************
typedef struct
{
    char *command;
    char *token;
    timer_t *timer;
    int state;
} at_command_t;
at_command_t at;
#define AT_STATE_READY 0   //!! Ready to received next command
#define AT_STATE_TOKEN 1   //!! Waiting for Token
#define AT_STATE_TIMEOUT 2 //!! Command was sent, but no resonse!

//!!*********************************
//!! Command Queue structure
//!!*********************************
#define COMD_LIST_LENGTH 32 //!! Maximum of commands
typedef struct
{
    char *pcomd[COMD_LIST_LENGTH];
    char *ptoken[COMD_LIST_LENGTH];
    int put;
    int get;
    int cnt;
} comd_list_t;
comd_list_t comdQueue;

//!! Initialize the comdQueue parameters
void ComdQueue_Init(void)
{
    comdQueue.put = 0;
    comdQueue.get = 0;
    comdQueue.cnt = 0;
}

//!! Put a command into the comdQueue
bool ComdQueue_Put(char *comd, char *token)
{
    if (comdQueue.cnt >= COMD_LIST_LENGTH)
    {
        //!! Queue FULL!!
        return false;
    }
    comdQueue.pcomd[comdQueue.put] = comd;
    comdQueue.ptoken[comdQueue.put] = token;
    comdQueue.put = (comdQueue.put + 1) % COMD_LIST_LENGTH;
    comdQueue.cnt++;
    return true;
}

//!! Get a command from the comdQueue
bool ComdQueue_Get(char **comd, char **token)
{
    if (comdQueue.cnt <= 0)
    {
        //!! Queue EMPTY!!
        return false;
    }
    *comd = comdQueue.pcomd[comdQueue.get];
    *token = comdQueue.ptoken[comdQueue.get];
    comdQueue.get = (comdQueue.get + 1) % COMD_LIST_LENGTH;
    comdQueue.cnt--;
    return true;
}

//!!*********************************
//!! AT command callbacks
//!!*********************************

//!! The command is sent and executed successfully
void AT_OnCommandCompleted(void *evt)
{
    //!! Kill the timout timer
    OS_TimerStop(at.timer);
    OS_TimerDelete(at.timer);
    at.timer = NULL;
}

//!! Timeout!, the command is sent, but no response!
void AT_OnCommandTimeout(void *evt)
{
    //!! Change at.state, Timeout!
    at.state = AT_STATE_TIMEOUT;

    //!! Kill the timeout timer
    OS_TimerStop(at.timer);
    OS_TimerDelete(at.timer);
    at.timer = NULL;

    //!! Report the timeout!
    Beep_FreqSet(100); //!! 100 Hz
    Beep(100);         //!! Beep sound
}

//!! Initialize AT
void AT_Init(void)
{
    at.state = AT_STATE_READY;
}

//!! Send a command to WiFi module
void AT_SendCommand(char *command, char *token, uint16_t timeout)
{
    //!! Update parameters
    at.command = command;
    at.token = token;

    //!! Send command to WiFi module
    Uart2_AsyncWriteString(at.command);

    //!! Change at.state, Waiting for Token
    at.state = AT_STATE_TOKEN;

    //!! Create timer for timeout checking
    if (at.timer != NULL)
    {
        OS_TimerStop(at.timer);
        OS_TimerDelete(at.timer);
        at.timer = NULL;
    }
    at.timer = OS_TimerCreate("at_to", timeout, 0, AT_OnCommandTimeout);
}

//!!*********************************
//!! WiFi Callbacks / For User
//!!*********************************

//!! OnConnected callback
void Wifi_OnConnected(void *evt)
{
    Uart1_AsyncWriteString(">> OnConnected\r\n");

    //!! Put your code here

    //!! Start server
    Uart1_AsyncWriteString(">> Starting Server...\r\n");
    ComdQueue_Put("AT+CIPSERVER=1,80\r\n", "OK\r\n");

    //!! Check IP address
    ComdQueue_Put("AT+CIFSR\r\n", "OK\r\n");
}

//!! OnDisconnected callback
void Wifi_OnDisconnected(void *evt)
{
    Uart1_AsyncWriteString(">> OnDisonnected\r\n");

    //!! Put your code here
}

//!! OnFailed callback. the Failed event normally occured
//!! when the WiFi module can not find the SSID on the first boot
#ifdef CHECK_WIFI_STATE_FAILED
void Wifi_OnFailed(void *evt)
{
    Uart1_AsyncWriteString(">> OnFailed\r\n");
    //!! Reset the Wifi module
    ComdQueue_Put("AT+RST\r\n", "OK\r\n");
}
#endif //!! CHECK_WIFI_STATE_FAILED


//!!
//!! Simple HTML page
//!!
const char *indexPage = \
"<html>\
<head>\
<style>\
.red{color:red;}\
.green{color:green;}\
.blue{color:blue;}\
</style>\
</head>\
<body>\
<h3>Welcome to IoT World!</h3>\
<h4>Supported commands:</h4>\
<ul>\
<li class='red'>IP/led0-on</li>\
<li class='red'>IP/led1-on</li>\
<li class='red'>IP/led2-on</li>\
<li class='red'>IP/led3-on</li>\
<li class='green'>IP/led0-off</li>\
<li class='green'>IP/led1-off</li>\
<li class='green'>IP/led2-off</li>\
<li class='green'>IP/led3-off</li>\
<li class='blue'>IP/beep</li>\
</ul>\
</body>\
</html>\
";

//!! Response message (new line) received callback
void WiFiModuleLineReceived(void *evt)
{
    //!! Take the received line
    data_t line_data = ((uart_event_t *)evt)->data;
    uint8_t *esp_data = line_data.buffer;

    if (!strcmp((const char *)esp_data, "ERROR\r\n"))
    {
        //Uart2_AsyncWriteString("AT+RST\r\n");
    }

    if (1)
    {
        if (str_index_of_first_token((const char *)esp_data, "+IPD,") >= 0)
        {
            //!! +IPD,0,468:GET /hello HTTP/1.1

            uint8_t idx1 = str_index_of_first_token((const char *)esp_data, ",") + 1;
            uint8_t channel = esp_data[idx1] - 0x30;

            #ifdef __DEBUG__
            static char buff[32];
            sprintf(buff, ">>Client connected at ch: %d\r\n", channel);
            Uart1_AsyncWriteString(buff);
            #endif

            uint8_t idx2 = str_index_of_first_token((const char *)esp_data, "GET /") + 5;
            uint8_t idx3 = str_index_of_first_token((const char *)esp_data, "HTTP/") - 1;

            #ifdef __DEBUG__
            sprintf(buff, "idx2:%d idx3:%d\r\n", idx2, idx3);
            Uart1_AsyncWriteString(buff);
            #endif

            if (idx2 >= 0 && idx3 >= 0)
            {
                //!! Take the GET message
                static char get[24] = {0};
                memcpy(get, esp_data + idx2, (idx3 - idx2));
                get[(idx3 - idx2)] = 0; // NULL terminate
                
                #ifdef __DEBUG__
                Uart1_AsyncWriteString(">>GET: [");
                Uart1_AsyncWriteString(get);
                Uart1_AsyncWriteString("]\r\n");
                #endif

                if (idx2 == idx3)
                {
                    //!! GET /
                    static char cipindex[32];
                    sprintf(cipindex, "AT+CIPSEND=%d,%d\r\n", channel, strlen(indexPage));
                    ComdQueue_Put(cipindex, "OK\r\n");
                    ComdQueue_Put(indexPage, "SEND OK\r\n");
                }
                else if (!strstr(get, "favicon.ico"))
                {
                    //!! Execute sime commands
                    if (!strcmp("led0-on", get))
                    {
                        LED0_On();
                    }
                    else if (!strcmp("led0-off", get))
                    {
                        LED0_Off();
                    }
                    else if (!strcmp("led1-on", get))
                    {
                        LED1_On();
                    }
                    else if (!strcmp("led1-off", get))
                    {
                        LED1_Off();
                    }
                    else if (!strcmp("led2-on", get))
                    {
                        LED2_On();
                    }
                    else if (!strcmp("led2-off", get))
                    {
                        LED2_Off();
                    }
                    else if (!strcmp("led3-on", get))
                    {
                        LED3_On();
                    }
                    else if (!strcmp("led3-off", get))
                    {
                        LED3_Off();
                    }
                    else if (!strcmp("beep", get))
                    {
                        Beep_FreqSet(2000);
                        Beep(100);
                    }

                    //!! Create response message
                    static char message[48];
                    sprintf(message, "Executed : %s", get);
                    int16_t mlength = strlen(message);

                    //!! Create CIPSEND
                    static char cipsend[32];
                    sprintf(cipsend, "AT+CIPSEND=%d,%d\r\n", channel, mlength);
                    ComdQueue_Put(cipsend, "OK\r\n");

                    // !!Send response message
                    ComdQueue_Put(message, "SEND OK\r\n");
                }

                //!! Close connection, the browser will update after this command
                static char cipclose[32];
                sprintf(cipclose, "AT+CIPCLOSE=%d\r\n", channel);
                ComdQueue_Put(cipclose, "OK\r\n");
            }
        }
    }

    //!!
    //!! AT command
    //!!
    if (at.state == AT_STATE_TOKEN)
    { // Waiting for token or timeout
        const char *token = (const char *)line_data.buffer;
        if (!strcmp(token, (const char *)at.token))
        {
            //!! Change at.state, Ready for next command
            at.state = AT_STATE_READY;
            AT_OnCommandCompleted((void *)0);
        }
    }

    //!!
    //!! WiFi connection
    //!!
    if (wifi.state == WIFI_STATE_DISCONNECTED)
    {
        if (!strcmp((const char *)esp_data, "WIFI GOT IP\r\n"))
        {
            //!! Change wifi.state, connected
            wifi.state = WIFI_STATE_CONNECTED;
            Wifi_OnConnected((void *)0);
        }

#ifdef CHECK_WIFI_STATE_FAILED
        else if (!strcmp(esp_data, "FAIL\r\n"))
        {
            //!! Change wifi.state, failed!
            wifi.state = WIFI_STATE_FAILED;
            Wifi_OnFailed((void *)0);
        }
#endif //!! CHECK_WIFI_STATE_FAILED
    }
    else if (wifi.state == WIFI_STATE_CONNECTED)
    {
        if (!strcmp((const char *)esp_data, "WIFI DISCONNECT\r\n"))
        {
            //!! Change wifi.state, disconnected!
            wifi.state = WIFI_STATE_DISCONNECTED;
            Wifi_OnDisconnected((void *)0);
        }

#ifdef CHECK_WIFI_STATE_FAILED
        else if (!strcmp(esp_data, "FAIL\r\n"))
        {
            //!! Change wifi.state, failed!
            wifi.state = WIFI_STATE_FAILED;
            Wifi_OnFailed((void *)0);
        }
#endif //!! CHECK_WIFI_STATE_FAILED
    }
#ifdef CHECK_WIFI_STATE_FAILED
    else if (wifi.state == WIFI_STATE_FAILED)
    {
        if (!strcmp(esp_data, "WIFI GOT IP\r\n"))
        {
            //!! Change wifi.state, connected
            wifi.state = WIFI_STATE_CONNECTED;
            Wifi_OnConnected((void *)0);
        }
        else if (!strcmp(esp_data, "WIFI DISCONNECT\r\n"))
        {
            //!! Change wifi.state, disconnected!
            wifi.state = WIFI_STATE_DISCONNECTED;
            Wifi_OnDisconnected((void *)0);
        }
    }
#endif //!! CHECK_WIFI_STATE_FAILED

    //!! Print received line to console/terminal
    Uart1_AsyncWriteString((const char *)esp_data);
    LED_Flash(LED_ID_3, 1);
}

//!! Initialize the WiFi module and try to connect to network
void WiFi_Init(const char *ssid, const char *pass)
{
    wifi.state = WIFI_STATE_DISCONNECTED;
    wifi.ssid = ssid;
    wifi.pass = pass;

    Uart1_AsyncWriteString("\r\n>> Conecting to network...\r\n");

    //!! Create connection command
    static char cmd[48]; //!! must be static or global
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", wifi.ssid, wifi.pass);
    ComdQueue_Put(cmd, "WIFI GOT IP\r\n");

    //!! Send commands to WiFi module
    ComdQueue_Put("ATE0\r\n", "OK\r\n");
    ComdQueue_Put("AT+CWMODE=1\r\n", "OK\r\n");
    ComdQueue_Put("AT+CIPMUX=1\r\n", "OK\r\n");
}

//!! Main Task/Service
void MainTask(void *evt)
{
    //!! Task ticks
    static uint32_t ticks = 0;
    static uint16_t disconnected_timeout = 0;
    ticks++;

    //!! AT command service
    char *pcomd, *ptoken;
    if (at.state == AT_STATE_READY)
    {
        if (ComdQueue_Get(&pcomd, &ptoken))
        {
            //!! Print information to console
            Uart1_AsyncWriteString("[AT] Sending >> ");
            Uart1_AsyncWriteString(pcomd);

            //!! Send command to WiFi module
            AT_SendCommand(pcomd, ptoken, 10000);
        }
    }
    else if (at.state == AT_STATE_TIMEOUT)
    {
        //!! Reset module
        Uart2_AsyncWriteString("AT+RST\r\n");

        //!! Wait long information message sending to console
        __delay_ms(1000);
    }

    //!! Connected
    if (wifi.state == WIFI_STATE_CONNECTED)
    {
        //!! Connected, LED3 flash 1 Hz
        if (!(ticks % 10))
        {
            LED_Flash(LED_ID_3, 2);
        }

        //!! Reset
        disconnected_timeout = 0;
    }
    //!! Disconnected
    else if (wifi.state == WIFI_STATE_DISCONNECTED)
    {
        //!! Disconnected!, LED3 flash 5 Hz
        if (!(ticks % 2))
        {
            LED_Flash(LED_ID_3, 2);
        }

        //!! Disconnected too long, 30 seconds, Reset the wifi module
        if (disconnected_timeout++ > 300)
        {
            //!! Reset
            disconnected_timeout = 0;

            //!! Restart
            Uart1_AsyncWriteString("Reseting the WiFi module...\r\n");
            ComdQueue_Put("AT+RST\r\n", "OK\r\n");

            //!! Reconnect
            Uart1_AsyncWriteString("Reconnect to WiFi Network...\r\n");
            WiFi_Init(SSID, PASS);
        }
    }
    //!!
    //!! Other services
    //!!
}

//!! Entry point
int main(void)
{
    //!! Initialize OS
    OS_Init();

    // Initialize AT
    AT_Init();

    //!! Initialize Command Queue
    ComdQueue_Init();

    //!! Beep on boot
    Beep(100);

    //!! Create a timer and register its callback
    OS_TimerCreate("service", 100, 1, MainTask);

    //!! Register UART2 Line Received Callback
    //!! The WiFi Module is connected to MCU via UART2
    OS_Uart2SetLineReceivedCallback(WiFiModuleLineReceived);

    //!! Initialize WiFi module and connect to network
    WiFi_Init(SSID, PASS);

    //!! Start OS
    OS_Start();
}
//!! EOF
