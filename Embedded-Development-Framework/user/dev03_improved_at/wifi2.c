
#include "config.h"
#include "wifi2.h"
#include "at2.h"
#include "os.h"

extern at_command_t at;

wifi_t wifi;






static void WiFi_PrintInfo( void ) {
    //!! TCP
    Uart1_AsyncWriteString( "\r\n-----------------------\r\n" );
    Uart1_AsyncWriteString( "TCP/IP:  ");
    Uart1_AsyncWriteString( wifi.ip);
    Uart1_AsyncWriteString( "\r\n");

    Uart1_AsyncWriteString( "Network: ");
    Uart1_AsyncWriteString( wifi.ssid);
    Uart1_AsyncWriteString( "\r\n-----------------------\r\n" );

    //!! AP
    // Uart1_AsyncWriteString( "SoftAP:  ");
    // Uart1_AsyncWriteString( wifi.ap_ip);
    // Uart1_AsyncWriteString( "\r\n");

    // Uart1_AsyncWriteString( "Network: ");
    // Uart1_AsyncWriteString( wifi.ap_ssid);
    // Uart1_AsyncWriteString( "\r\n-----------------------\r\n" );

    Beep( 50 );
 } //!! WiFi_PrintInfo()

static void WiFi_UpdateIpMac( const char *line )
{
    if( line[7] == 'S' )
    {
        //!!
        //!! Station
        //!!
        // recv: +CIFSR:STAIP,"192.168.1.131"
        // recv: +CIFSR:STAMAC,"2c:3a:e8:11:1e:95"
        uint8_t idx1 = str_index_of_first_char( line, '"' );
        uint8_t idx2 = str_index_of_last_char( line, '"' );
        idx1 += 1;
        if( line[10] == 'I' ) 
        {
            memcpy( wifi.ip, line + (idx1), (idx2-idx1) );
        }
        else if( line[10] == 'M') 
        {
            memcpy( wifi.mac, line + (idx1), (idx2-idx1) );  
        }
    }
    else if( line[7] == 'A' )
    {
        //!!
        //!! SoftAP
        //!!
        // recv: +CIFSR:APIP,"192.168.0.123"
        // recv: +CIFSR:APMAC,"2e:3a:e8:11:1e:95"

        //!! Do nothing for now
    }
} //!! WiFi_UpdateIpMac()


void WiFi_Reconnect(void);
void WiFi_Service(void)
{
    static uint16_t prv_sec = 0;
    uint16_t cur_sec = OS_TimeGet().ss;

    if( prv_sec != cur_sec ) {   
        prv_sec = cur_sec;

        if( wifi.state == WIFI_STATE_DISCONNECTED ) {
            //Uart1_AsyncWriteString( "WIFI: No internet network...\r\n" );
        }
        else if( wifi.state == WIFI_STATE_CONNECTED ) {
            //Uart1_AsyncWriteString( "WIFI: Waiting for IP address...\r\n" );
        }
        else if( wifi.state == WIFI_STATE_GOT_IP ) {
            
        }
        else if( wifi.state == WIFI_STATE_STARTING_SRV ) {
            
        }
        else if( wifi.state == WIFI_STATE_SERVER_RUNNING ) {
            
        }
    }

    if( wifi.state == WIFI_STATE_STARTING_SRV ) 
    {
        if( at.returnCode == AT_RETURN_ERROR ||  at.returnCode == AT_RETURN_FAIL )
        {
            //!!
            //!! In this point, the server cannot be start. Reconnect again
            //!!
            wifi.state = WIFI_STATE_DISCONNECTED;


            AT_CleanMemory();   //!! Reject the rast commands and cleanup

            WiFi_Reconnect();   //!! Reconnect to the network
        }
    }
}


worker_t *server_start_worker = NULL;

void Worker_StartHTTPServer(void *evt) 
{
    event_t  *event  = (event_t *)evt;
    worker_t *worker = (worker_t *)event->sender;

    if(worker->state == 0)  {
         
        Uart1_AsyncWriteString( "\r\nWIFI: Starting HTTP Server...\r\n" );
        AT_Put( "AT+CIPMUX=1\r\n" ); 
        worker->state++;
        OS_WorkerSleep(worker, 500);
    }
    else if(worker->state == 1) {
        AT_Put( "AT+CIPSERVER=1,80\r\n" );
        worker->state++;
        OS_WorkerSleep(worker, 500);
    }
    else if(worker->state == 2) {
        //!! Ask for IP and MAC addresses of the ICP/IP server
        AT_Put( "AT+CIFSR\r\n" );
        worker->state++;
        OS_WorkerDelete(worker);
        server_start_worker = NULL;
        wifi.state = WIFI_STATE_SERVER_RUNNING;
    }
}

void WiFi_ProcessLine(const char *line)
{
    if( 0 == str_index_of_first_token(line, "WIFI DISCONNECT\r\n") )  {
        wifi.state = WIFI_STATE_DISCONNECTED;
    }
    else if( 0 == str_index_of_first_token(line, "WIFI CONNECTED\r\n") )  {
        wifi.state = WIFI_STATE_CONNECTED;
    }
    else if( 0 == str_index_of_first_token(line, "WIFI GOT IP\r\n") )  {
        wifi.state = WIFI_STATE_GOT_IP;

        //--------------------------------------------------------------------
        Uart1_AsyncWriteString( "WIFI: Got IP. Starting HTTP Server...\r\n" );
        wifi.state = WIFI_STATE_STARTING_SRV;

        if(server_start_worker != NULL) {
            OS_WorkerDelete(server_start_worker);
            server_start_worker = NULL;    
        }
        server_start_worker = OS_WorkerCreate("start", Worker_StartHTTPServer);

    }
    else if( 0 == str_index_of_first_token( line, "+CIFSR:APIP," )      ||
             0 == str_index_of_first_token( line, "+CIFSR:APMAC," )     ||
             0 == str_index_of_first_token( line, "+CIFSR:STAIP," )     ||
             0 == str_index_of_first_token( line, "+CIFSR:STAMAC," ) )  {
        
        //!! Update IP and MAC addresses
        WiFi_UpdateIpMac(line);

        //!! Print Information
        if( wifi.ip[0] != 0 && wifi.mac[0] != 0 )  {
            WiFi_PrintInfo();
        }
    }
    else {
        //
    }
}

void WiFi_Reconnect(void)
{

    wifi.state = WIFI_STATE_DISCONNECTED;

    Uart1_AsyncWriteString("\r\n-------------------------------------\r\n");
    Uart1_AsyncWriteString(" Connecting to network \"");
    Uart1_AsyncWriteString(wifi.ssid);
    Uart1_AsyncWriteString("\"....\r\n");
    Uart1_AsyncWriteString("-------------------------------------\r\n\r\n");

    char *pCWMODE = malloc( 32 );
    sprintf( pCWMODE,"AT+CWMODE=%d\r\n", wifi.mode );
    AT_Put( pCWMODE );

    char *pCWJAP = malloc( strlen(wifi.ssid) + strlen(wifi.pass) + 16 );
    sprintf( pCWJAP, "AT+CWJAP=\"%s\",\"%s\"\r\n", wifi.ssid, wifi.pass );
    AT_Put( pCWJAP );
}

void WiFi_Init(const char *ssid, const char *pass, uint8_t mode) 
{
    wifi.ssid = ssid;
    wifi.pass = pass;
    wifi.mode = mode;

    //AT_Put( "AT+RST\r\n" );
    AT_Put( "ATE0\r\n" );
    AT_Put( "AT+GMR\r\n" );

    WiFi_Reconnect();
}
