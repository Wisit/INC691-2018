
#include "os.h"
#include "server.h"

const char *SSID = "WLRT02";            //!! SSID
const char *PASS = "WLRT11112";         //!! PASS


void GetCallback(void *evt) 
{
    server_t *server = (server_t *)evt;

    if(server->client != NULL)
    {
        server->client->data = "IKJSHCJKS";
    }
}
int main(void) 
{
    OS_Init();                          //!! Initial the OS
    HTTP_ServerInit(SSID, PASS, GetCallback);  //!! Initialise the HTTP Server
    OS_Start();                         //!! Start OS
}
