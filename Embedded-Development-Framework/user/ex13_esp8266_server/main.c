
#include "os.h"
#include "http.h"

const char *SSID = "WLRT02";     //!! SSID
const char *PASS = "WLRT11112";     //!! PASSWORD


//!! Callback function for GET command
void GetCallback(void *evt) 
{
    //!! Take the server
    server_t *server = (server_t *)evt;

    //!! Is the server has its target, the client?
    if(server->target) 
    {   
        //!! Print the get command passing vai the browser
        char buff[32];
        char *pGET = server->target->getCommand;
        sprintf(buff, "GET:%s\r\n", pGET);
        Uart1_AsyncWriteString(buff);

        //!! Do some simple command checking
        if(!strcmp(pGET, "led0-on")) 
        {
            LED0_On();  // Turn ON LED0
        }
        else if(!strcmp(pGET, "led0-off")) 
        {
            LED0_Off(); // Turn OFF LED0
        }
        else if(!strcmp(pGET, "led3-inv")) 
        {
            LED3_Inv(); // Invert/Toggle LED3
        }
        else if(!strcmp(pGET, "beep")) 
        {
            Beep(100);  // Beep 0.1 seconds
        }
    }
}

int main(void) 
{
    //!! Initial the OS
    OS_Init();

    //!! Initial the Server
    HTTP_ServerInit(SSID, PASS, GetCallback);

    //!! Start OS
    OS_Start();
}


