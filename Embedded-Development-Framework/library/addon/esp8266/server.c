
#include "server.h"
#include "at.h"
#include "wifi.h"

//!! Server messages for debugging
const char *SrvStateMsg[] = {
    "[0] SERVER_STATE_STOPPED",
    "[1] SERVER_STATE_STARTED"};


//!! Global object/structure
server_t server;


void Server_Init(os_callback_t stateChangedCallback)
{
    server.state = SERVER_STATE_STARTED;
    server.stateChangedCallback = stateChangedCallback;
}



#define Server_ApplyStateChanged(newState) {            \
    server.state = newState;                            \
    if(server.stateChangedCallback != NULL) {           \
        server.stateChangedCallback((void *)&server);   \
    }                                                   \
}

uint16_t Server_ProcessLine(const char *line)
{

    //!! Package format: 
    //!!    +IPD,0,468 / hello / HTTP1.1
    if(server.state == SERVER_STATE_STARTED)
    {
        if (str_index_of_first_token(line, "+IPD,") == 0)
        {

            Beep_FreqSet(4000);
            Beep(10);

            uint8_t idx1 = str_index_of_first_token(line, ",") + 1;
            uint8_t channel = line[idx1] - 0x30;

            //!! GET[1 SPACE]/
            uint8_t idx2 = str_index_of_first_token(line, "GET /")  + 5;
            
            //!! HTTP/
            uint8_t idx3 = str_index_of_first_token(line, "HTTP/") - 1;

        
            //!! Check
            if(idx2 >= 0 && idx3 >=0 ) {
                //!! Copy the GET content
                char getBuffer[32];
                memcpy(getBuffer, line + idx2, (idx3 - idx2));
                getBuffer[(idx3 - idx2)] = NULL;    // NULL terminate

                if (!strcmp(getBuffer, "favicon.ico"))
                    return -1;

                //!!
                server.connectionChannel = channel;
                strcpy(server.getCommand, getBuffer);
                Server_ApplyStateChanged(SERVER_STATE_GET_REQUEST);

                char buff[48];
                sprintf(buff, "\r\nserver.state: %d %d %s\r\b", server.state, server.connectionChannel, server.getCommand);
                Uart1_AsyncWriteString(buff);
            }
        }
        else if (line, "SEND OK\r\n")
        {
           
        } 
    }
    return server.state;
}


void Server_Loop(void *evt)
{
    //!! Server should be stopped if there is no ip address
    if(wifi.state != WIFI_STATE_IP_MAC_UPDATED) {
        LED_Flash(LED_ID_3, 10);

        //!!
        //!! The Server should be started of the the wifi.state is WIFI_STATE_IP_MAC_UPDATED
        //!!

        //!! DO NOT stop the Server at this point!! because it nerver be started
        //server.state = SERVER_STATE_STOPPED;
        //return;
    }

    //!! Received package header
    if (server.state == SERVER_STATE_GET_REQUEST)
    {
        //!!
        Server_ApplyStateChanged(SERVER_STATE_MAKE_RESPONSE);
    }
    //!! Make response to connected client
    else if (server.state == SERVER_STATE_MAKE_RESPONSE)
    {
        //!! Generate a response message
        os_time_t time = OS_TimeGet();
        static char message[128];
        sprintf(message, "<html><body><h1>%.2d:%.2d:%.2d</h1></body></html>", time.hh, time.mm, time.ss);

        //!! Create CIPSEND command
        static char comd[32]; //char *comd = (char *)malloc(32);
        sprintf(comd, "AT+CIPSEND=%d,%d\r\n", server.connectionChannel, strlen(message));
        
        //!! Sent the command
        AT_PutCommand((char *)comd);
        
        //!! Sent the message
        AT_PutCommand((char *)message); //!! SEND OK

        //!!
        Server_ApplyStateChanged(SERVER_STATE_END_CONNECTION);
    }
    //!! Close the connection
    else if (server.state == SERVER_STATE_END_CONNECTION)
    {
        static char cmdClose[32]; //char *cmdClose = (char *)malloc(32);
        sprintf(cmdClose, "AT+CIPCLOSE=%d\r\n", server.connectionChannel);
        AT_PutCommand((char *)cmdClose); //!!??

        //!!
        Server_ApplyStateChanged(SERVER_STATE_STARTED);
    }

    LED_Flash(LED_ID_0, 20);
}

