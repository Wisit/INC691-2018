#include "os.h"

//const char * Mqtt_Host  = "198.41.30.241"
const char * Mqtt_Host   = "iot.eclipse.org";
const char * Mqtt_Port   = "1883";
const char * Protocol    = "MQTT";
const char * Mqtt_Topic  = "Ecc";
const char * Client_Name = "EccLab";


//!!
//#define LINE_RECEIVED_DEBUG   1
//#define WIFI_CONTROLLER_DEBUG 1
//#define MQTT_FRAME_DEBUG      1   

void Mqtt_Connect(void);
void MqttSubscript(void);

typedef uint8_t wifi_state_t;
#define WIFI_STATS_DISCONNECTED (wifi_state_t)0
#define WIFI_STATS_CONNECTED    (wifi_state_t)1
#define WIFI_STATS_GOT_IP       (wifi_state_t)2
wifi_state_t wifi_state;


#define COM_SendString  Uart1_AsyncWriteString
#define COM_SendBytes   Uart1_AsyncWriteBytes

// Sends a AT command to the esp8266 module
#define ESP_SendCommand Uart2_AsyncWriteString


typedef uint8_t wifi_at_status_t;
#define AT_STATE_CMD_READY      (wifi_at_status_t)0   // ready to receive a new command
#define AT_STATE_CMD_SENT       (wifi_at_status_t)1   // command is sent to the module
#define AT_STATE_CMD_PROC       (wifi_at_status_t)2   // in progress, waiting for response
#define AT_STATE_CMD_DONE       (wifi_at_status_t)3   // response message is received ot timeout!


typedef uint8_t wifi_at_result_t;
#define AT_CMD_RET_SUCCESS      (wifi_at_result_t)0   
#define AT_CMD_RET_TIMEOUT      (wifi_at_result_t)1
#define AT_CMD_RET_FAILED       (wifi_at_result_t)2
#define AT_CMD_RET_UNDEFINED    (wifi_at_result_t)3

typedef char *      wifi_at_command_t;
typedef uint16_t    wifi_at_timeout_t;

wifi_at_status_t    at_state     = AT_STATE_CMD_READY;
char                at_comd_buf[256];       // buffer for at command used in WiFi_ATGetCommand
wifi_at_command_t   at_token_ptr = 0;       // pointer to command token
wifi_at_timeout_t   at_timeout   = 0;       // tick counter
wifi_at_result_t    at_result    = AT_CMD_RET_UNDEFINED;



#define AT_TIMEOUT_VALUE    5000        // timeout value in mS (1 mS/tick)

uint32_t at_ts, at_te;
uint8_t at_token_flag = 0;


#define ES_AT_COMMAND_DEBUG     1


uint8_t WiFi_ATGetStatus(void) {
    return at_state;
}

uint8_t WiFi_ATGetResult(void) {
    return at_result;
}

#define AT_COMD_BUFF_LENGTH 1024
char at_comd_queue[AT_COMD_BUFF_LENGTH];
uint16_t at_comd_put = 0;
uint16_t at_comd_get = 0;
uint16_t at_comd_cnt = 0;

uint8_t WiFi_ATPutCommand(char *comd) {
    int space = AT_COMD_BUFF_LENGTH - at_comd_cnt;
    int length = str_length(comd);
    if(space < length) {
        return 0;
    }
    char *str = at_comd_queue + at_comd_put;
    int i;
    for(i=0; i<length; i++) {
        *str++ = *comd++;
        at_comd_cnt++;
        at_comd_put++;
        if(at_comd_put >= AT_COMD_BUFF_LENGTH) {
            at_comd_put = 0;
            str = at_comd_queue + 0;  // re-assign the pointer to the most left, [0]
        }
    }
    return 1;
}

uint8_t WiFi_ATGetCommand(char *comd) {
    if(at_comd_cnt <= 0) {
        at_comd_put = 0;    // reset pointers
        at_comd_get = 0;
        return 0;
    }
    
    char *q = at_comd_queue + at_comd_get;
    char *p = comd;
    int length = 0;
    while(*q != '\n') {
        length++; q++;
    }

    int i;
    q = at_comd_queue + at_comd_get;
    for(i=0; i<=length; i++) {
        *p++ = *q++;
        at_comd_cnt--;
        at_comd_get++;
        if(at_comd_get >= AT_COMD_BUFF_LENGTH) {
            at_comd_get = 0;
            q = at_comd_queue + 0;  // re-assign the pointer to the most left, [0]
        }
    }
    *p = 0;
    return 1;
}
// Must be called every 1 mS
void WiFi_ATService(void) {
    switch(at_state) {
        case AT_STATE_CMD_READY:    // Ready to receive next command
            if(WiFi_ATGetCommand(at_comd_buf)) {
                #ifdef ES_AT_COMMAND_DEBUG
                    at_ts = OS_TimeMicroseconds();
                    Uart1_AsyncWriteString("\r\nAT-CMD: Sending command ");
                    Uart1_AsyncWriteString(at_comd_buf);
                #endif

                at_timeout = 0;                     // reset timeout
                at_state   = AT_STATE_CMD_SENT;     // command is sent to the module
                at_state   = AT_STATE_CMD_PROC;     // the command is now processing
                at_result  = AT_CMD_RET_UNDEFINED;  // waiting for result, will be updated lather
                Uart2_AsyncWriteString(at_comd_buf);
            }
            break;
            
        case AT_STATE_CMD_SENT:     // The command is sent to the module, 
            break;

        case AT_STATE_CMD_PROC:     // The command is executing, waiting for response message
            at_timeout++;           // if no response message is returned, 
            if(at_timeout >= AT_TIMEOUT_VALUE) {
                at_result   = AT_CMD_RET_TIMEOUT;   // timeout, no response is received
                at_state    = AT_STATE_CMD_DONE;    // change state to AT_STATE_CMD_DONE
            }
            break; 
            
        case AT_STATE_CMD_DONE:     // The response message is received
            
            if(at_result == AT_CMD_RET_SUCCESS) {
                #ifdef ES_AT_COMMAND_DEBUG
                    Uart1_AsyncWriteString("\r\nAT-CMD: command ");
                    Uart1_AsyncWriteString(at_comd_buf);
                    Uart1_AsyncWriteString("SUCCESS!");
                #endif
            }
            else if(at_result == AT_CMD_RET_TIMEOUT) {
                #ifdef ES_AT_COMMAND_DEBUG
                    Uart1_AsyncWriteString("\r\nAT-CMD: command ");
                    Uart1_AsyncWriteString(at_comd_buf);
                    Uart1_AsyncWriteString("TIMEOUT!");
                #endif
            }
            else if(at_result == AT_CMD_RET_FAILED) {
                #ifdef ES_AT_COMMAND_DEBUG
                    Uart1_AsyncWriteString("\r\nAT-CMD: command ");
                    Uart1_AsyncWriteString(at_comd_buf);
                    Uart1_AsyncWriteString("FAILED!");
                #endif
            }
            #ifdef ES_AT_COMMAND_DEBUG
                char buff[32];
                at_te = OS_TimeMicroseconds();
                sprintf(buff, " -- %ld uS", at_te - at_ts);
                Uart1_AsyncWriteString(buff);
            #endif

            at_state    = AT_STATE_CMD_READY;   // change state to AT_STATE_CMD_READY 

            break;

        default:
            break;
    }
}

typedef uint8_t mqtt_state_t;
#define MQTT_STATE_DISCONNECTED (mqtt_state_t)0
#define MQTT_STATE_CONNECTED    (mqtt_state_t)1
#define MQTT_STATE_SUBSCRIPTED  (mqtt_state_t)2
mqtt_state_t mqtt_state = MQTT_STATE_DISCONNECTED;

void WiFi_LineReceived(void *evt) {
    
    uart_event_t *e = (uart_event_t *)evt;
    //Uart1_AsyncWriteData(&e->data);
    
    uint8_t *line   = e->data.buffer;
    uint16_t length = e->data.length;
    
    /***************************************************************************/
    /*                       AT-Command State Controller                       */
    /***************************************************************************/
    
    #ifdef LINE_RECEIVED_DEBUG
        COM_SendString("\r\nLine Received: ");
        COM_SendBytes(line, length);
    #endif

    #if 0
    if(at_token_ptr) {
        if(-1 != str_raw_index_of_first_token((char *)line->data, line->length, (char *)at_token_ptr)) {
            at_token_flag = 1;
        }
    }
    #endif
    
    if(-1 != str_raw_index_of_first_token((char *)line, length, (char *)"OK\r\n")) {
        #ifdef LINE_RECEIVED_DEBUG
            COM_SendString("\r\nToken OK received\r\n");
        #endif
        if(at_state  == AT_STATE_CMD_PROC) {
            at_state  = AT_STATE_CMD_DONE;  // done with
            at_result = AT_CMD_RET_SUCCESS; // success
            //str_raw_copy((char *)line->data, line->length, at_comd_buf);
        }
    }
    else if(-1 != str_raw_index_of_first_token((char *)line, length, (char *)"ERROR\r\n")) {
        #ifdef LINE_RECEIVED_DEBUG
            COM_SendString("\r\nToken ERROR received\r\n");
        #endif
        if(at_state == AT_STATE_CMD_PROC) {
            at_state  = AT_STATE_CMD_DONE;  // done with
            at_result = AT_CMD_RET_FAILED; // failed (error)
        }
    }

    
    /***************************************************************************/
    /*                         WiFi State Controller                           */
    /***************************************************************************/

    else if(-1 != str_raw_index_of_first_token((char *)line, length, (char *)"WIFI DISCONNECT\r\n")) {
        wifi_state = WIFI_STATS_DISCONNECTED;
    }
    else if(-1 != str_raw_index_of_first_token((char *)line, length, (char *)"WIFI CONNECTED\r\n")) {
        wifi_state = WIFI_STATS_DISCONNECTED;
    }
    else if(-1 != str_raw_index_of_first_token((char *)line, length, (char *)"WIFI GOT IP\r\n")) {
        wifi_state = WIFI_STATS_GOT_IP;
    }

    
    /***************************************************************************/
    /*                    Internet Connection Controller                       */
    /***************************************************************************/
    else if(-1 != str_raw_index_of_first_token((char *)line, length, (char *)"CONNECT\r\n")) {
        mqtt_state = MQTT_STATE_CONNECTED;
    }
    else if(-1 != str_raw_index_of_first_token((char *)line, length, (char *)"CLOSED\r\n")) {
        mqtt_state = MQTT_STATE_DISCONNECTED;
    }
        
    else if(-1 != str_raw_index_of_first_token((char *)line, length, (char *)"+IPD,")) { /* !! No \r\n*/
        
    }   
}



void WiFi_Controller(void *evt) {

    event_t *event = (event_t *)evt;
    worker_t *worker = (worker_t *)event->sender;
    //!!
    //!! the wifi_statas is modified by the WiFi_LineReceived callback
    //!!
    switch(wifi_state) {
        case WIFI_STATS_DISCONNECTED:
            //!!
            //!! Waiting for WiFi connection
            //!!
            #ifdef WIFI_CONTROLLER_DEBUG
                Uart1_AsyncWriteString("\r\nWiFi Controller: no wifi, reseting module...");
            #endif

            WiFi_ATPutCommand("AT+RST\r\n");
        break;
        
        case WIFI_STATS_CONNECTED:
            //!!
            //!! Waiting for IP Address
            //!!
            #ifdef WIFI_CONTROLLER_DEBUG
                Uart1_AsyncWriteString("\r\nWiFi Controller: no ip, reseting module...");
            #endif

            WiFi_ATPutCommand("AT+RST\r\n");
        break;
            
        case WIFI_STATS_GOT_IP:
            //!!
            //!! Got IP address, ready to online
            //!!
            #ifdef WIFI_CONTROLLER_DEBUG
                Uart1_AsyncWriteString("\r\nWiFi Controller: received ip, online...");
            #endif
        break;
    }
    
    OS_WorkerSleep(worker, 10000);
}


void WiFi_FrameReceived(void *evt);

void MQTT_Controller(void *evt) {
     event_t *event = (event_t *)evt;
    worker_t *worker = (worker_t *)event->sender;
    
    //!! if no ip address is received, return
    if(wifi_state != WIFI_STATS_GOT_IP) {
        Uart1_AsyncWriteString("\r\nMQTT Controller: waiting for IP address...");
        OS_WorkerSleep(worker, 2000);
        return;
    }
     
    
    //!! the mqtt_state will be modified in the WiFi_LineReceived callback
    switch(mqtt_state) {
        case MQTT_STATE_DISCONNECTED:{
            Uart1_AsyncWriteString("\r\nMQTT Controller: connecting to mqtt server...");
            WiFi_ATPutCommand("AT+CIPMUX=1\r\n");
            //WiFi_ATPutCommand("AT+CIPSTART=0,\"TCP\",\"198.41.30.241\",1883\r\n");
            char buff[64];
            sprintf(buff, "AT+CIPSTART=0,\"TCP\",\"%s\",%s\r\n", Mqtt_Host, Mqtt_Port);
            WiFi_ATPutCommand(buff);
            //WiFi_ATPutCommand("AT+CIPSTART=0,\"TCP\",\"iot.eclipse.org\",1883\r\n");
            //
        }
        break;
        
        case MQTT_STATE_CONNECTED: {
            Uart1_AsyncWriteString("\r\nMQTT Controller: connected to mqtt server.");
            worker->state = 0;
            Mqtt_Connect(); 
            mqtt_state = MQTT_STATE_SUBSCRIPTED; 
            OS_WorkerSleep(worker, 2000);
            return;
        }
        break;
        
        case MQTT_STATE_SUBSCRIPTED:{
            if(worker->state == 0) {
                worker->state = 1;
                MqttSubscript();
                Uart1_AsyncWriteString("\r\nMQTT Controller: subscript to mqtt server.");
                OS_WiFiSetFrameReceivedCallback(WiFi_FrameReceived);
            }
        }
        break;
    }
    
    OS_WorkerSleep(worker, 5000);
}



void MqttPing(void *event_data) {
    worker_t * worker = (worker_t *)event_data;
    if( worker->state == 0) {
        Uart1_AsyncWriteString("\r\n-----------<<PING>>-----------\r\n");
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
}

void Mqtt_Connect(void) {
    uint8_t mqtt[64];
    uint8_t i=0;
    mqtt[i++] = 0x10;           // Connect
    mqtt[i++] = 0x00;           // Length of header frame, will be update later
    mqtt[i++] = 0x00;           // Protocol Name Length MSB
    
    //mqtt[i++] = 0x04;           // Protocol Name Length LSB (4:=MQTT)
    mqtt[i++] = str_length(Protocol);
    
    //mqtt[i++] = (uint8_t)'M';
    //mqtt[i++] = (uint8_t)'Q';
    //mqtt[i++] = (uint8_t)'T';
    //mqtt[i++] = (uint8_t)'T';
    char *p = (char *)Protocol;
     while(*p)
        mqtt[i++] = *p++;
    
    mqtt[i++] = 0x04;           // Version
    mqtt[i++] = 0x02;           // Connection flag
    mqtt[i++] = 0x00;           // Keep alive time MSB
    mqtt[i++] = 0x0A;           // Keep alive time LSB (10 sec)
    mqtt[i++] = 0x00;           // Client Name Length MSB
    //mqtt[i++] = 0x06;           // Client Name Length (6:=EccLab)
    mqtt[i++] = str_length(Client_Name);
    
    p = (char *)Client_Name;
    while(*p)
        mqtt[i++] = *p++;
    //mqtt[i++] = (uint8_t)'E';
    //mqtt[i++] = (uint8_t)'c';
    //mqtt[i++] = (uint8_t)'c';
    //mqtt[i++] = (uint8_t)'L';
    //mqtt[i++] = (uint8_t)'a';
    //mqtt[i++] = (uint8_t)'b';
    
    mqtt[1]   = i-2;
    char cmd_buffer[32];
    sprintf(cmd_buffer, "AT+CIPSEND=0,%d\r\n", i);
    Uart2_AsyncWriteString(cmd_buffer); __delay_ms(1000);
    Uart2_AsyncWriteBytes(mqtt, i);
    
   
    // Ping timer
    OS_WorkerCreate("Ping", MqttPing);
}

void MqttSubscript(void) {
    uint8_t mqtt[64];
    uint8_t i = 0;
    
    mqtt[i++] = 0x82;               // Subscript
    mqtt[i++] = 0x00;               // Remain Length, will be updated below  
    mqtt[i++] = 0xaa;               // Message ID MSB  
    mqtt[i++] = 0xbb;               // Message ID LSB  
    mqtt[i++] = 0x00;               // Topic Name Length MSB  
    mqtt[i++] = 0x03;               // Topic Name Length LSB (3:=Ecc) 
    
    //mqtt[i++] = (uint8_t)'E';       // 'E', Topic Name, 'Ecc'  
    //mqtt[i++] = (uint8_t)'c';       // 'c', Topic Name, 'Ecc'  
    //mqtt[i++] = (uint8_t)'c';       // 'c', Topic Name, 'Ecc'
    char *p = (char *)Mqtt_Topic;
    while(*p) 
        mqtt[i++] = *p++;
    
    mqtt[i++] = 0x00;               // Requested QoS : 0  
    mqtt[1]   = i-2;
    char cmd_buffer[32];
    sprintf(cmd_buffer, "AT+CIPSEND=0,%d\r\n", i);
    Uart2_AsyncWriteString(cmd_buffer); __delay_ms(500);
    Uart2_AsyncWriteBytes(mqtt, i);
}


void WiFi_FrameReceived(void *evt) {
    uart_line_t * line = (uart_line_t *)evt;
    
    #ifdef MQTT_FRAME_DEBUG
        Uart1_AsyncWriteString("\r\nMQTT Package: ");
        Uart1_AsyncWriteBytes(line->data, line->length);
    #endif
    
    int i1 = str_raw_index_of_first_token((const char *)line->data, line->length, (const char *)"Ecc");
    int i2 = line->length-1;
    Uart1_AsyncWriteString("\r\nMQTT DATA:");
    Uart1_AsyncWriteBytes(line->data+i1+1, i2-i1);
}

int main(void)
{
    
    
    OS_Init();
    
    
    Uart1_AsyncWriteString("\r\nSystem Ready!\r\n");


    OS_Uart2SetLineReceivedCallback(WiFi_LineReceived); // required for WiFi_ATService
    //OS_WiFiSetFrameReceivedCallback(WiFi_FrameReceived);
    OS_WorkerCreate("WiFi-Controller", WiFi_Controller);
    OS_WorkerCreate("MQTT-Controller", MQTT_Controller);

    
    LED_PwmSet(LED_ID_0, 1000,   0, 1);
    LED_PwmSet(LED_ID_1, 1000, 100, 1);
    LED_PwmSet(LED_ID_2, 1000, 200, 1);
    LED_PwmSet(LED_ID_3, 1000, 300, 1);

    unsigned int k = 0;
    while(1) {
        OS_Execute();
        OS_EventLoop();
        if(OS_TickedCheck()) {
           
            if(++k%50 == 0) {
                LED2_Inv();
                WiFi_ATService();
            }
        }
    }
}