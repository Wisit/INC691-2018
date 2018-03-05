#include "http.h"
#include "os.h"



const char *SSID = "WLRT02";    //!! SSID
const char *PASS = "WLRT11112"; //!! Password


at_command_t at;
client_t clients[CLIENT_CONNECTION_MAX];
server_t server;
esp_init_t esp;

void AT_Init(void) 
{
    at.state = AT_STATE_READY;
    at.timeoutTicks = 0;
    at.returnType = AT_RETURN_UNKNOWN;
}


void Client_Init(void) 
{
    uint8_t i;
    client_t *p = clients;
    for(i=0; i<CLIENT_CONNECTION_MAX; i++) {
        p->connectionState = CLIENT_STATE_DISCONNECTED;
        p->channel = 0xFF;
        p->getCommand[0] = 0;
        p++;
    }
}



void Server_Init(void)
{
    server.target  = NULL;
    server.timer   = NULL;
    server.clients = clients;
}


void ESP_Init(void) {
    esp.state = 0;
    esp.nextState = 0;
    esp.timer = NULL;
}






void Server_Loop(void *evt) {
    os_time_t time;
    char buff[48];
    static uint8_t  prev_sec   = 0;
    static uint8_t  loopState  = 0;
    static uint8_t  next_state = 0;  //!! Must be initialized as zero
    static uint16_t next_responeType = 0xFF;
    uint8_t i;
   
    //!! Read system time
    time = OS_TimeGet();

    if(server.state == SERVER_STATE_IDLE) 
    {   
        client_t *c = &clients[0]; //!! Point to the first client
        for(i=0; i<CLIENT_CONNECTION_MAX; i++) 
        {
            if(c->connectionState == CLIENT_STATE_REQUESTED) {
                server.target = c;
                server.state = SERVER_STATE_BUSY;
                break;
            }
            c++;    //!! Next client
        }     
    }

    if(server.target == NULL) {
        //!! No more connected clients
        LED_Flash(LED_ID_3, 2);
        return;
    }

    //!! In progress
    LED_Flash(LED_ID_1, 2);

    //!! Point to the current client
    client_t *client = server.target;


    
    static char page[32];

    static uint8_t loopTimeout = 0;

    //!! Send CIPSEND (send header to ESP)
    if( loopState == 0 ) 
    {
        //!! Response content
        sprintf(page, "Time: %.2d:%.2d:%.2d", time.hh, time.mm, time.ss);


        sprintf(buff, "AT+CIPSEND=%d,%d\r\n", client->channel, strlen(page));
        at.state = AT_STATE_WAIT_RESPONSE;  //!! Change the at.state to make it wait for response message 
        next_responeType = AT_RETURN_OK;    //!! Prefered returned message
        Uart2_AsyncWriteString(buff);       //!! Send command/data to the ESP
        loopState = 0xFF;                       //!! Next time it will go to the last case
        next_state++;                       //!! Next state, it will be reloaded (written to state) 

        #ifdef _SERVER_DEBUG_
        Uart1_AsyncWriteString("AT: ");
        Uart1_AsyncWriteString(buff);
        #endif
    }
    //!! Send Content (file/page)
    else if( loopState == 1 )
    {
        at.state = AT_STATE_WAIT_RESPONSE;
        next_responeType = AT_RETURN_SEND_OK;   //!! Waiting for SEND_OK
        Uart2_AsyncWriteString(page);
        loopState = 0xFF;
        next_state++;

        #ifdef _SERVER_DEBUG_
        Uart1_AsyncWriteString("AT: ");
        Uart1_AsyncWriteString(page);
        Uart1_AsyncWriteString("\r\n");
        #endif
    } 
    //!! Send CIPCLOSE (close connection)
    else if( loopState == 2 )
    {
        at.state = AT_STATE_WAIT_RESPONSE;
        next_responeType = AT_RETURN_OK;
        sprintf(buff, "AT+CIPCLOSE=%d\r\n", client->channel);
        Uart2_AsyncWriteString(buff);
        loopState = 0xFF;
        next_state++;

        #ifdef _SERVER_DEBUG_
        Uart1_AsyncWriteString("AT: ");
        Uart1_AsyncWriteString(buff);
        #endif
    }
    //!!The connection is closed
    else if( loopState == 3 ) {

        //!! Clear the client
        client->connectionState = CLIENT_STATE_DISCONNECTED;

        //!! Reset the at
        at.state = AT_STATE_READY;

        //!! Reset the server
        server.target = NULL;
        server.state = SERVER_STATE_IDLE;

        //!! Reset states and internal variables
        loopState      = 0;
        next_state = 0;

    }
    //!! Waiting state
    else
    {   
        if(at.state == AT_STATE_GOT_RESPONSE) 
        {   //!! Got response OK or SEND_OK from the module
            //if( (at.returnType == AT_RETURN_OK)  || 
            //    (at.returnType == AT_RETURN_SEND_OK)) 
            {   //!! Prefered response message is received,
                
                //!! Move to next state
                loopState    = next_state;
            }
            //!! Reset the at
            at.state = AT_STATE_READY;

            //!! Reset timout
            loopTimeout = 0;
        }
        else
        {
            //!! Timeout checking
            //LED_Flash(LED_ID_3, 10);

            if(prev_sec != time.ss) 
            {
                prev_sec = time.ss;
                sprintf(buff, "%d: return:%d wait:%d\r\n", time.ss, at.returnType, next_responeType);
                Uart1_AsyncWriteString(buff);
                if(loopTimeout++ > 5) {
                    loopTimeout = 0;

                    //!! Restart the ESP module
                    Uart2_AsyncWriteString("AT+RST\r\n"); 
                    
                    //!! Restart the server
                    __delay_ms(100);
                    HTTP_ServerInit();
                    
                }
            }
        }
    }
}




void ESPLineReceived(void *evt) {
    

    char buff[48]; //!! buffer for sprintf()

    //!! Get received line
    const char *line = (const char *)((uart_event_t *)evt)->data.buffer;


    //!! Default return type
    at.returnType = AT_RETURN_UNKNOWN;


    //!!
    //!! AT Returned Messages
    //!!    1) "OK\r\n"
    //!!    2) "SEND OK\r\n"
    //!!
    if(!strcmp(line, "OK\r\n")) {
        at.state = AT_STATE_GOT_RESPONSE;
        at.returnType = AT_RETURN_OK;
    }
    else if(!strcmp(line, "ready\r\n")) {
        at.state = AT_STATE_GOT_RESPONSE;    
        at.returnType = AT_RETURN_ESP_READY; 
    }
    else if(!strcmp(line, "SEND OK\r\n")) {
        at.state = AT_STATE_GOT_RESPONSE;    
        at.returnType = AT_RETURN_SEND_OK; 
    }
    else if(!strcmp(line, "SEND FAIL\r\n")) {
        at.state = AT_STATE_GOT_RESPONSE;    
        at.returnType = AT_RETURN_SEND_FAIL; 
    }
     else if(!strcmp(line, "busy s...\r\n")) {
        at.state = AT_STATE_GOT_RESPONSE;    
        at.returnType = AT_RETURN_BUSY; 
    }
    else if(!strcmp(line, "no change\r\n")) {
        at.state = AT_STATE_GOT_RESPONSE;    
        at.returnType = AT_RETURN_NO_CHANGE; 
    }
    else if(!strcmp(line, "FAIL\r\n")) {
        at.state = AT_STATE_GOT_RESPONSE;    
        at.returnType = AT_RETURN_FAIL;
        Uart1_AsyncWriteString("AT+RST\r\n");
        Beep(10);
    }
    else if(!strcmp(line, "link is builded\r\n")) {
        at.state = AT_STATE_GOT_RESPONSE;    
        at.returnType = AT_RETURN_LINK_BUILDED;
    }
    else if(!strcmp(line, "ERROR\r\n")) {
        at.state = AT_STATE_GOT_RESPONSE;    
        at.returnType = AT_RETURN_ERROR;
    }


    //!!
    //!! WIFI Messages
    //!!    1) "WIFI DISCONNECT\r\n"
    //!!    2) "WIFI CONNECTED\r\n"
    //!!    3) "WIFI GOT IP\r\n"
    //!!
    else if(!strcmp(line, "WIFI DISCONNECT\r\n")) {

    }
    else if(!strcmp(line, "WIFI CONNECTED\r\n")) {

    }
    else if(!strcmp(line, "WIFI GOT IP\r\n")) {
        
    }

    //!!
    //!! Client Messages
    //!!    1) x,CONNECTED\r\n
    //!!    2) "+IPD,<???>\r\n
    //!!    3) x,CLOSED\r\n   
    //!!
    else if( 1 == str_index_of_first_token(line, ",CONNECT") ) {
        //!! Client connected
        
    }
    else if( 0 == str_index_of_first_token(line, "+IPD,") ) {
        //!! Client GET requested

        //!!
        //!! Format: +IPD,0,419:GET / HTTP/1.1
        //!!
        uint8_t channel = line[5] - 0x30;
        uint8_t idx1 = str_index_of_first_token(line, "GET /");
        uint8_t idx2 = str_index_of_first_token(line, "HTTP/");
        if(idx1<0 || idx2 <0 || channel<0 || channel >4) {
            //!! Wrong Format!!
            sprintf(buff, "\r\nWrong Format! idx1:%d idx2:%d channel:%d\r\n", idx1, idx2, channel);
            Uart1_AsyncWriteString(buff);
            Beep(1000);
        }
        char getCommand[32];
        idx1 += 5;
        idx2 -= 1;
        memset(getCommand, 0, 32);
        memcpy( getCommand, (line + idx1), (idx2 - idx1) );

        #ifdef _ESP_LINE_RECEIVED_
        sprintf(buff, "Client[%d] requests with [%s]\r\n", channel, getCommand);
        Uart1_AsyncWriteString(buff);
        #endif


        //!! Create the connected client
        client_t *p = &clients[channel];
        p->channel = channel;
        sprintf(p->getCommand, "%s", getCommand);
        p->connectionState = CLIENT_STATE_REQUESTED;
        if(server.timer == NULL) {
            server.timer = OS_TimerCreate("server", 50, 1, Server_Loop);  
        }
    }
    else if( 1 == str_index_of_first_token(line, ",CLOSED") ) {
        //!! Client closed/disconnected

    }

    #ifdef _ESP_LINE_RECEIVED_
    //!! Print unknown return type
    //if(at.returnType != AT_RETURN_UNKNOWN) 
    {
        Uart1_AsyncWriteString("recev: ");
        Uart1_AsyncWriteString(line);
    }
    #endif
}





void ESP_InitLoop(void *evt) {
    static uint8_t next_state = 0;
    if( esp.state == 0 ) {
        at.state = AT_STATE_WAIT_RESPONSE;
        Uart2_AsyncWriteString("ATE0\r\n");
        next_state = 1;
        esp.state = 0xFF;

        #ifdef _ESP_LINE_RECEIVED_
        Uart1_AsyncWriteString("AT: ATE0\r\n");
        #endif
        
    }
    else if( esp.state == 1 ) {
        at.state = AT_STATE_WAIT_RESPONSE;
        Uart2_AsyncWriteString("AT+CWMODE=1\r\n");
        next_state++;
        esp.state = 0xFF;

        #ifdef _ESP_LINE_RECEIVED_
        Uart1_AsyncWriteString("AT: AT+CWMODE=1\r\n");
        #endif
    }
    else if( esp.state == 2 ) {
        char buff[48];
        sprintf(buff, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS);
        at.state = AT_STATE_WAIT_RESPONSE;
        Uart2_AsyncWriteString(buff);
        next_state++;
        esp.state = 0xFF;

        #ifdef _ESP_LINE_RECEIVED_
        Uart1_AsyncWriteString("AT: ");
        Uart1_AsyncWriteString(buff);
        #endif
    }
    else if( esp.state == 3 ) {
        at.state = AT_STATE_WAIT_RESPONSE;
        Uart2_AsyncWriteString("AT+CIPMUX=1\r\n");
        next_state++;
        esp.state = 0xFF;

        #ifdef _ESP_LINE_RECEIVED_
        Uart1_AsyncWriteString("AT: AT+CIPMUX=1\r\n");
        #endif
    }
    else if( esp.state == 4 ) {
        at.state = AT_STATE_WAIT_RESPONSE;
        Uart2_AsyncWriteString("AT+CIPSERVER=1,80\r\n");
        next_state++;
        esp.state = 0xFF;

        #ifdef _ESP_LINE_RECEIVED_
        Uart1_AsyncWriteString("AT: AT+CIPSERVER=1,80\r\n");
        #endif
    }
    else if( esp.state == 5 ) {
        //!! Ask for IP and MAC addresses
        at.state = AT_STATE_WAIT_RESPONSE;
        Uart2_AsyncWriteString("AT+CIFSR\r\n");
        next_state++;
        esp.state = 0xFF;

        #ifdef _ESP_LINE_RECEIVED_
        Uart1_AsyncWriteString("AT: AT+CIFSR\r\n");
        #endif
    }
    else if( esp.state == 6 ) {
        OS_TimerDelete(esp.timer);     
        esp.timer = NULL;
        esp.state = 0; 
        at.state = AT_STATE_READY;

        #ifdef _ESP_LINE_RECEIVED_
        Uart1_AsyncWriteString("Done!!\r\n");
        #endif

        //!! Ask for IP and MAC addresses
        Uart1_AsyncWriteString("AT+CIFSR\r\n");
    }
    else{
        if(at.state == AT_STATE_GOT_RESPONSE) {
            esp.state = next_state;
            at.state = AT_STATE_READY;
        }
    }

    LED_Flash(LED_ID_0, 1);
}

//

void HTTP_ServerInit(void)
{
    if(esp.timer != NULL) {
        OS_TimerDelete(esp.timer); 
        esp.timer = NULL;  
    }   
    if(server.timer != NULL) {
        OS_TimerDelete(server.timer); 
        server.timer = NULL;     
    }
    AT_Init();
    ESP_Init();
    Client_Init();
    Server_Init();

    Uart2_ClearRingBuffer();
    OS_Uart2SetLineReceivedCallback(ESPLineReceived);
    esp.timer = OS_TimerCreate("esp-init", 100, 1, ESP_InitLoop);
}

