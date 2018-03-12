#include "os.h"
#include "str.h"

//!!
//!! NOTE: Ping is required before Subscript
//!!


//const char * Mqtt_Host  = "198.41.30.241"
const char * Mqtt_Host   = "iot.eclipse.org";
const char * Mqtt_Port   = "1883";
const char * Protocol    = "MQTT";
const char * Mqtt_Topic  = "Ecc";
const char * Client_Name = "EccLab";





void Worker_InternetConnect(void *evt);
void Worker_ESPInitialise(void *evt);

void Worker_MqttConnect(void *evt);
void Worker_MqttPing(void *evt);
void Worker_MqttSubscript(void *evt);

worker_t *worker_mqtt_connect = NULL;
worker_t *worker_mqtt_ping = NULL;
worker_t *worker_mqtt_subscript = NULL;

//!! state machine for AT command operations
#define AT_STATE_READY      1   //!! ready to receive new command
#define AT_STATE_BUSY       0   //!! execuiting command

#define AT_TIMEOUT_VALUE    50  //!! timeout
typedef struct {
    uint8_t  state;             //!! AT state
    uint16_t timeout;           //!! timeout counter
}at_command_t;
at_command_t at;


#define INTERNET_STATUS_OFFLINE     0
#define INTERNET_STATUS_ONLINE      1

typedef struct {
    uint8_t status;
}internet_t;

internet_t internet;

void Internet_Init(void) 
{
    internet.status = INTERNET_STATUS_OFFLINE;
}

#define WIFI_STATUS_DISCONNECTED    0
#define WIFI_STATUS_CONNECTED       1
#define WIFI_STATUS_GOT_IP          2

typedef struct {
    uint8_t status;
}wifi_t;
wifi_t wifi;

void WiFi_Init(void) {
    wifi.status = WIFI_STATUS_DISCONNECTED;
}



#define ESP_STATUS_INIT     0
#define ESP_STATUS_READY    1
typedef struct {
    uint8_t status;
    uint8_t cmd_idx;
}esp_t;
esp_t esp;

void ESP_Init(void) {
    esp.cmd_idx = 0;
    esp.status = ESP_STATUS_INIT;
}


//!! initial at parameter(s)
void AT_Init(void)  {
    
    at.state   = AT_STATE_READY;
    at.timeout = 0;
}

//!! periodically called by timer every 100 mS
void AT_Service(void *evt) {
    
    (void)evt;

    if(at.state == AT_STATE_READY) 
    {

    }
    else if(at.state == AT_STATE_BUSY) 
    {
        if(at.timeout++ >= AT_TIMEOUT_VALUE) {
            at.timeout = 0;
            Beep(50);
            at.state = AT_STATE_READY;
            UART1_AsyncWriteString("AT-Timeout!\r\n");
        }
    }
}

//!! sends command to WiFi module and set at.state to AT_STATE_BUSY
void AT_CommandSend(char *command) {
    
    at.state = AT_STATE_BUSY;
    LED_Flash(LED_ID_0, 5);
    Uart2_AsyncWriteString(command);
}

//!! will be called when the at.state is AT_STATE_BUSY only
uint16_t AT_CheckToken(char *line, uint16_t length) {
    
    if(-1 != str_raw_index_of_first_token(line, length, "OK\r\n")) 
    {
        at.state = AT_STATE_READY;
        Uart1_AsyncWriteString("AT-Token [OK]\r\n");
        return 1;
    }
    else if(-1 != str_raw_index_of_first_token(line, length, "ready\r\n")) 
    {
        at.state = AT_STATE_READY;
        Uart1_AsyncWriteString("AT-Token [ready]\r\n");
        return 1;
    }
    else if(-1 != str_raw_index_of_first_token(line, length, "SEND OK\r\n"))
    {
        at.state = AT_STATE_READY;
        Uart1_AsyncWriteString("AT-Token [SEND OK]\r\n");
        return 1;
    }
    else if(-1 != str_raw_index_of_first_token(line, length, "ERROR\r\n")){
        at.state = AT_STATE_READY;
        Uart1_AsyncWriteString("AT-Token [ERROR]\r\n");
        Beep(10);
        return 0;
    }
    else if(-1 != str_raw_index_of_first_token(line, length, "busy p...\r\n")) 
    {
        return 0;
    }
    else {

    }
    return 0;
}

//!! continuously called to check WiFi status
int16_t  WiFi_CheckStatus(char *line, uint16_t length)  {
    
    if(-1 != str_raw_index_of_first_token(line, length, "WIFI CONNECTED\r\n")) 
    {
        //!! connected to access point (AP) and waiting for IP address
        wifi.status = WIFI_STATUS_CONNECTED;
        return 1;
    }
    else if(-1 != str_raw_index_of_first_token(line, length, "WIFI GOT IP\r\n")) 
    {
        //!! received IP address
        wifi.status = WIFI_STATUS_GOT_IP;
        OS_WorkerCreate("Internet", Worker_InternetConnect);
        return 1;
    }
    else if(-1 != str_raw_index_of_first_token(line, length, "WIFI DISCONNECT\r\n")) 
    {
        //!! no WiFi or no AP is detected
        wifi.status = WIFI_STATUS_DISCONNECTED;
        return 0;
    }
    return 0;
}

//!! continuously called to check internet connection status
int16_t Internet_CheckStatus(char *line, uint16_t length) {
    
    if(-1 != str_raw_index_of_first_token(line, length, "0,CONNECT\r\n")) 
    {
        //!! connected to server
        internet.status = INTERNET_STATUS_ONLINE;
        UART1_AsyncWriteString("Internet: Connected to MQTT Server\r\n");
        
        if(worker_mqtt_connect == NULL)
            worker_mqtt_connect = OS_WorkerCreate("MQTTConnect", Worker_MqttConnect);

        Beep(5);
        return 1;
    }  
    else if(-1 != str_raw_index_of_first_token(line, length, "ALREADY CONNECTED\r\n")) 
    {
        //!! reconnected to server
        internet.status = INTERNET_STATUS_ONLINE;
        
        if(worker_mqtt_connect == NULL)
            worker_mqtt_connect = OS_WorkerCreate("MQTTConnect", Worker_MqttConnect);
        return 1;
    } 
    else if(-1 != str_raw_index_of_first_token(line, length, "0,CLOSED\r\n")) 
    {
        //!! server disconnected
        internet.status = INTERNET_STATUS_OFFLINE;
        return 0;
    } 
    else if(-1 != str_raw_index_of_first_token(line, length, "0,CONNECT FAIL\r\n")) 
    {
        //!! no WiFi or no IP, but try to connect to server
        internet.status = INTERNET_STATUS_OFFLINE;
        return 0;
    }
    return 0;
}


uint8_t line_debug = 0;

//!! called when a new line is received
void ESP_LineReceived(void *evt)  {
    
    uart_event_t *e = (uart_event_t *)evt;
    uint8_t *line   = e->data.buffer;
    uint16_t length = e->data.length;
    
    if(line_debug)
        UART1_AsyncWriteBytes(line, length);
    
    //!!
    //!! AT Command
    //!!
    if(at.state == AT_STATE_BUSY) 
    {
        //!! check returned message form AT operations
        if(AT_CheckToken((char *)line, length)) 
        {
            //!! ESP Initialisation AT commands
            if(esp.status == ESP_STATUS_INIT) 
            {
                esp.cmd_idx++;
                if(esp.cmd_idx >= 3) { // we have 3 initial commands, AT+RST, AT+CWMODE=3 and AT+CIPMUX=1
                    esp.status = ESP_STATUS_READY;
                    esp.cmd_idx = 0;
                }
            }
        }
    }
    
    //!!
    //!! WiFi
    //!!
    WiFi_CheckStatus((char *)line, length);
    
    //!!
    //!! Internet
    //!!
    Internet_CheckStatus((char *)line, length);

}

//!! activated when IP address is obtained
void Worker_InternetConnect(void *evt) {
  
    event_t *event = (event_t *)evt;
    worker_t *worker = (worker_t *)event->sender;
    
    if(wifi.status != WIFI_STATUS_GOT_IP) {
        Uart1_AsyncWriteString("Worker_InternetConnect is deleted\r\n");
        OS_WorkerDelete(worker);
        return;
    }
    
    if(internet.status == INTERNET_STATUS_OFFLINE) {
        Uart1_AsyncWriteString("Connecting to 198.41.30.241...\r\n");
        AT_CommandSend("AT+CIPSTART=0,\"TCP\",\"198.41.30.241\",1883\r\n");
        OS_WorkerSleep(worker, 3000);
        return;
    }
    else if(internet.status == INTERNET_STATUS_ONLINE) {
        //!! do nothing
        LED_Flash(LED_ID_3, 5);
        OS_WorkerSleep(worker, 500);
        return;
    }
    
    OS_WorkerSleep(worker, 2000);
    return;
}

//!! activated in main function
void Worker_ESPInitialise(void *evt) {

    event_t *event = (event_t *)evt;
    worker_t *worker = (worker_t *)event->sender;
    
    if(at.state == AT_STATE_BUSY)
    {
        OS_WorkerSleep(worker, 100);
        return;
    }
    
    if(esp.status == ESP_STATUS_INIT)
    {
        if(esp.cmd_idx == 0) {
            AT_CommandSend("AT+RST\r\n");
        }
        else if(esp.cmd_idx == 1) {
            AT_CommandSend("AT+CWMODE=3\r\n");
        }
        else if(esp.cmd_idx == 2) {
            AT_CommandSend("AT+CIPMUX=1\r\n");
        }
        
        
        OS_WorkerSleep(worker, 500);
        return;
    }

    if(wifi.status != WIFI_STATUS_GOT_IP) {
        Uart1_AsyncWriteString("Waiting for IP Address...\r\n");
        
        OS_WorkerSleep(worker, 3000);
        return;
    }

    OS_WorkerSleep(worker, 1000);
}

void ESP_WiFiFrameReceived(void *evt) {
    uart_event_t *e = (uart_event_t *)evt;
    uint8_t *line   = e->data.buffer;
    uint16_t length = e->data.length;

    
    LED_Flash(LED_ID_2, 1);
    
    int i1 = str_raw_index_of_first_token((char *)line, length, "Ecc");
    
    char *target = (char *)(line+i1+3); // "Ecc"=3
    length  = length-i1-3;
    
    UART1_AsyncWriteString("\r\n\r\nMQTT Data:<START>");
    UART1_AsyncWriteBytes((uint8_t *)target, length);
    UART1_AsyncWriteString("<END>\r\n\r\n");
    
    if(-1 != str_raw_index_of_first_token(target, length, "led0-on")) {
        LED0_On();
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led1-on")) {
        LED1_On();
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led0-off")) {
        LED0_Off();
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led1-off")) {
        LED1_Off();
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led0-inv")) {
        LED0_Inv();
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led1-inv")) {
        LED1_Inv();
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led0-flash")) {
        LED_Flash(0, 500);
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led1-flash")) {
        LED_Flash(1, 500);
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led-on")) {
        LED0_On();
        LED1_On();
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led-off")) {
        LED0_Off();
        LED1_Off();
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led-inv")) {
        LED0_Inv();
        LED1_Inv();
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "led-flash")) {
       LED_Flash(0, 500);
       LED_Flash(1, 500);
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "beep")) {
        Beep(100);
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "debug-on")) {
        line_debug = 1;
    }
    else if(-1 != str_raw_index_of_first_token(target, length, "debug-off")) {
        line_debug = 0;
    }
    
}


//!! activated when internet.state is changed INTERNET_STATUS_ONLINE
void Worker_MqttConnect(void *evt) {
    
    event_t  *event  = (event_t *)evt;
    worker_t *worker = (worker_t *)event->sender;

    uint8_t mqtt[64];
    uint8_t i=0;
    mqtt[i++] = 0x10;           // Connect
    mqtt[i++] = 0x00;           // Length of header frame, will be update later
    mqtt[i++] = 0x00;           // Protocol Name Length MSB       
    mqtt[i++] = str_length(Protocol);   // Protocol Name Length LSB (4:=MQTT)
    char *p = (char *)Protocol; // Protocol name, "MQTT"
    while(*p)
        mqtt[i++] = *p++;
    
    mqtt[i++] = 0x04;           // Version
    mqtt[i++] = 0x02;           // Connection flag
    mqtt[i++] = 0x00;           // Keep alive time MSB
    mqtt[i++] = 0x0A;           // Keep alive time LSB (10 sec)
    mqtt[i++] = 0x00;           // Client Name Length MSB
    mqtt[i++] = str_length(Client_Name);    // Client Name Length (6:=EccLab)
    
    p = (char *)Client_Name;    // Client name
    while(*p)
        mqtt[i++] = *p++;

    mqtt[1]   = i-2;            // Length of header frame
    
    if(worker->state == 0) {
        char cmd_buffer[32];
        sprintf(cmd_buffer, "AT+CIPSEND=0,%d\r\n", i);
        Uart2_AsyncWriteString(cmd_buffer);// __delay_ms(1000);
        worker->state++;
        OS_WorkerSleep(worker, 500);
        return;
    }
    else {
 
        //!! send header
        Uart2_AsyncWriteBytes(mqtt, i);
        
        //!! delete worker
        OS_WorkerDelete(worker);
        worker_mqtt_connect = NULL;
        
        //!! activate WiFi callback
        OS_WiFiSetFrameReceivedCallback(ESP_WiFiFrameReceived);
        
        
        //!!        
        if(worker_mqtt_subscript == NULL)
            OS_WorkerCreate("Subscript", Worker_MqttSubscript);

        //!! create ping worker
        if(worker_mqtt_ping == NULL)
            OS_WorkerCreate("Ping", Worker_MqttPing);
        
        return;
    }
}


void Worker_MqttSubscript(void *evt) {

    
    event_t  *event  = (event_t *)evt;
    worker_t *worker = (worker_t *)event->sender;
    
    //!! this worker (subscript-worker) and ping-worker are created as the same time
    //!! but Ping operation must be applied be the Subscript operation can be sent
    //!! wait for 2 seconds to be sure the ping-worker complete its tasks
    if( worker->state == 0) {
        OS_WorkerSleep(worker, 2000);
        worker->state++;
        return;
    }
    
    uint8_t mqtt[32];
    uint8_t i = 0;
    
    mqtt[i++] = 0x82;               // Subscript
    mqtt[i++] = 0x00;               // Remain Length, will be updated below  
    mqtt[i++] = 0xAA;               // Message ID MSB  
    mqtt[i++] = 0xBB;               // Message ID LSB  
    mqtt[i++] = 0x00;               // Topic Name Length MSB  
    mqtt[i++] = 0x03;               // Topic Name Length LSB (3:=Ecc) 

    char *p = (char *)Mqtt_Topic;   // Topic name
    while(*p) 
        mqtt[i++] = *p++;
    
    mqtt[i++] = 0x00;               // Requested QoS : 0  
    mqtt[1]   = i-2;                // Remain Length
    
    if(worker->state == 1) {
        char cmd_buffer[32];
        sprintf(cmd_buffer, "AT+CIPSEND=0,%d\r\n", i);
        Uart2_AsyncWriteString(cmd_buffer);
        
        worker->state++;
        OS_WorkerSleep(worker, 500);
        return;
    }
    else {
        //!! send subscript-frame
        Uart2_AsyncWriteBytes(mqtt, i);
        
        //!! delete the worker
        OS_WorkerDelete(worker);
    }
}


void Worker_MqttPing(void *evt) {
    
    event_t  *event  = (event_t *)evt;
    worker_t *worker = (worker_t *)event->sender;
   
    if( worker->state == 0) {
        Uart1_AsyncWriteString("\r\n<<MQTT Client PING>>\r\n");
        Uart2_AsyncWriteString("AT+CIPSEND=0,2\r\n");
        worker->state++;
        OS_WorkerSleep(worker, 500);
        return;
    }
    uint8_t mqtt[2];
    uint8_t i = 0;
    mqtt[i++] = 0xc0;
    mqtt[i++] = 0x00;
    Uart2_AsyncWriteBytes(mqtt, i);
    
    worker->state = 0;
    OS_WorkerSleep(worker, 9500);
    
    LED_Flash(LED_ID_2, 10);
}

int main(void)  
{
    OS_Init();
    AT_Init();
    ESP_Init();
    WiFi_Init();
    Internet_Init();
    OS_TimerCreate("AT_Service", 100, 1, AT_Service);
    OS_WorkerCreate("WiFi_Init", Worker_ESPInitialise);
    OS_Uart2SetLineReceivedCallback(ESP_LineReceived);
    UART1_AsyncWriteString("\r\nMQTT Client...\r\n");
    Beep(100);
    OS_Start();
}



