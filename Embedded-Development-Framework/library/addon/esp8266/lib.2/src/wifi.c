/************************************************************
 * File:    wifi.c                                          *
 * Author:  Dr.Santi Nuratch                                *
 *          Embedded Computing and Control Laboratory       *
 * Update:  09 March 2018, 06.53 PM                         *
 ************************************************************/

#include "wifi.h"


//!! Global variable/object
wifi_t wifi;


#define WiFi_ApplyStateChanged(newState) {  \
    wifi.state = newState;                  \
    if( wifi.callback != NULL ) {           \
        wifi.callback(&wifi);               \
    }                                       \
}

static void WiFi_StartServer(void) 
{
    AT_Put( "AT+CIPMUX=1\r\n" ); 
    AT_Put( "AT+CIPSERVER=1,80\r\n" );

    //!! Ask for IP and MAC addresses of the ICP/IP server
    AT_Put( "AT+CIFSR\r\n" );
}// !! WiFi_StartServer()

static void WiFi_StartSoftAP(void)
{
    if( ( wifi.mode & WIFI_MODE_SOFT_AP ) != 0) 
    {
        char *pBuff = malloc( strlen(wifi.ap_ssid) + strlen(wifi.ap_pass) + 22 );
        sprintf( pBuff, "AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n", wifi.ap_ssid, wifi.ap_pass, wifi.ap_chan, wifi.ap_encr );
        AT_Put( pBuff );

        char *pSoftIp = malloc(48);
        sprintf( pSoftIp, "AT+CIPAP=\"%s\"\r\n", wifi.ap_ip );
        AT_Put( pSoftIp );


        //!! Ask of AP_SSID and AP_IP of the SoftAP
        AT_Put( "AT+CWSAP?\r\n" ); 
        AT_Put( "AT+CIPAP?\r\n" ); 
    }
} // WiFi_StartSoftAP()

static void WiFi_UpdateIpMac(const char *line)
{
    if(line[7] == 'S')
    {
        //!!
        //!! Station
        //!!
        // recv: +CIFSR:STAIP,"192.168.1.131"
        // recv: +CIFSR:STAMAC,"2c:3a:e8:11:1e:95"
        uint8_t idx1 = str_index_of_first_char(line, '"');
        uint8_t idx2 = str_index_of_last_char(line, '"');
        idx1 += 1;
        if(line[10] == 'I') 
        {
            memcpy(wifi.ip, line + (idx1), (idx2-idx1) );
        }
        else if(line[10] == 'M') 
        {
            memcpy(wifi.mac, line + (idx1), (idx2-idx1) );  
        }
    }
    else if(line[7] == 'A')
    {
        //!!
        //!! SoftAP
        //!!
        // recv: +CIFSR:APIP,"192.168.0.123"
        // recv: +CIFSR:APMAC,"2e:3a:e8:11:1e:95"

        //!! Do nothing for now
    }
} //!! WiFi_UpdateIpMac()

 static void WiFi_PrintInfo(void)
 {
    //!! TCP
    Uart1_AsyncWriteString("\r\n-----------------------\r\n");
    Uart1_AsyncWriteString("TCP/IP:  ");
    Uart1_AsyncWriteString(wifi.ip);
    Uart1_AsyncWriteString("\r\n");

    Uart1_AsyncWriteString("Network: ");
    Uart1_AsyncWriteString(wifi.ssid);
    Uart1_AsyncWriteString("\r\n-----------------------\r\n");

    //!! AP
    Uart1_AsyncWriteString("SoftAP:  ");
    Uart1_AsyncWriteString(wifi.ap_ip);
    Uart1_AsyncWriteString("\r\n");

    Uart1_AsyncWriteString("Network: ");
    Uart1_AsyncWriteString(wifi.ap_ssid);
    Uart1_AsyncWriteString("\r\n-----------------------\r\n");

    Beep(50);
 } //!! WiFi_PrintInfo()



//!! Must be called by LineReceived callback
void WiFi_Service(void *evt)
{
    const char *line = (const char *)evt;

     //!! Called by Timer
    if( line == NULL ) 
    {  
        return;
    }
    //!! Called by LineReceived
    else if( !strcmp( line, "WIFI DISCONNECT\r\n" ) ) 
    {
        WiFi_ApplyStateChanged( WIFI_STATE_DISCONNECTED );
    }
    else if( !strcmp( line, "WIFI CONNECTED\r\n" ) ) 
    {
        WiFi_ApplyStateChanged( WIFI_STATE_CONNECTED );
    }
    else if( !strcmp( line, "WIFI GOT IP\r\n" ) ) 
    {
        WiFi_ApplyStateChanged( WIFI_STATE_GOT_IP_ADDR );
        
        //!! Start Server
        WiFi_StartServer();

        //!! Start SoftAP
        WiFi_StartSoftAP();
    }
    else if( 0 == str_index_of_first_token( line, "+CIFSR:APIP," )  ||
             0 == str_index_of_first_token( line, "+CIFSR:APMAC," ) ||
             0 == str_index_of_first_token( line, "+CIFSR:STAIP," ) ||
             0 == str_index_of_first_token( line, "+CIFSR:STAMAC," ) )
    {
        //!! Update IP and MAC addresses
        WiFi_UpdateIpMac(line);

        //!! Print Information
        if( wifi.ip[0] != 0 && wifi.mac[0] != 0 )
        {
            WiFi_PrintInfo();
        }
    }
} //!! WiFi_Service()


void WiFi_InitStation( const char *ssid, const char *pass, uint8_t mode)
{
    wifi.ssid       = ssid;
    wifi.pass       = pass;
    wifi.ip[0]      = 0;
    wifi.mac[0]     = 0;
    wifi.mode       = mode;
    wifi.state      = WIFI_STATE_DISCONNECTED;
} //!! WiFi_InitStation()


void WiFi_InitSoftAP( const char *ap_ssid, const char *ap_pass, const char *ap_ip, uint8_t ap_chan, uint8_t ap_encr ) 
{
    if( ( wifi.mode & WIFI_MODE_SOFT_AP ) == 0 ) 
    {
        Uart1_AsyncWriteString("\r\nWarning: The module is not set to SoftAP mode!\r\n");
    }

    //!!-------------------------------------------------------------
    wifi.ap_ssid = ap_ssid;
    wifi.ap_pass = ap_pass;
    wifi.ap_chan = ap_chan;
    wifi.ap_encr = ap_encr;
    wifi.ap_ip   = ap_ip;
} //!! WiFi_InitSoftAP()


void WiFi_Start( os_callback_t callback )
{
    wifi.callback   = callback;
    
    //!!----------------------------------------------------
    //!! Print selected operation mode 
    const char *WiFiModeMsg[]={"0:Unknown","1:Station","2:SoftAP","3:Station+SoftAP"};
    Uart1_AsyncWriteString("Setting WiFi to mode \"");
    Uart1_AsyncWriteString(WiFiModeMsg[wifi.mode]);
    Uart1_AsyncWriteString("\"...\r\n");
    
    //!! Send the CWMODE command
    char buff[32];
    sprintf( buff,"AT+CWMODE=%d\r\n", wifi.mode );
    AT_Put( buff );  

    //!!----------------------------------------------------
    //!! Print SSID
    Uart1_AsyncWriteString("Connecting to network \"");
    Uart1_AsyncWriteString(wifi.ssid);
    Uart1_AsyncWriteString("\"...\r\n");

    //!! Send the CWJAP command
    char *pBuff = malloc( strlen(wifi.ssid) + strlen(wifi.pass) + 16 );
    sprintf( pBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", wifi.ssid, wifi.pass );
    AT_Put( pBuff );
}// !! WiFi_Start()
