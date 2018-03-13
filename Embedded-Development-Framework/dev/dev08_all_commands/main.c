
#include "os.h"
#include "at2.h"
#include "wifi2.h"
#include "server2.h"

//!!--------------------------------------------------------------------------------------------
const char      *SSID       =   "WLRT02";
const char      *PASS       =   "WLRT11112";

//!!--------------------------------------------------------------------------------------------
//!! SoftAP
const char      *AP_SSID    = "ECC-SoftAP";             //!! SoftAP/Host SSID
const char      *AP_PASS    = "embedded";               //!! 8-character or longer are required
const char      *AP_IPADDR  = "192.168.0.1";            //!! IP address
const uint8_t   AP_CHANNEL  = 2;                        //!! 2, 3, 4, 5 work (0, 1 not work)
const uint8_t   AP_ENCRYPT  = WIFI_AP_ENCRYPT_WPA_PSK;  //!! Encryption type

//!!--------------------------------------------------------------------------------------------
//!! Note: In SoftAP, using external files, e.g.; jQuery, Bootstrap and others
//!!       are not posible. However. those files can be loaded from the cache of the browser
//!!       To make the files saved in the cache, just enter IP address of the server 
//!!       (IP address of Station, not SoftAP) 
//!!--------------------------------------------------------------------------------------------


//!!
//!! led:d:d;
//!!

#define print(s) Uart1_AsyncWriteString(s)




int get_id_value( const char *line, int is, int ie ) {
    char buff[10];
    int n = ie - is - 1;
    if( n >= 8 ){
        return -1;
    }
    memcpy(buff, line + is + 1, n);
    buff[n] = 0; // Null terminate
    return( atoi( buff ) );   
}

char * Command_Executor(const char *line)
{
    int idx1 = str_index_of_first_char(line, '/');
    int idx2 = str_index_of_last_char (line, '/');
    int idx3 = str_index_of_first_char(line, ';');

    //int len = strlen(line);

    
    //!! START Debug ------------------------------------
    // print("Process: ");
    // print(line);
    // print("\r\n");
    // char buff[32];
    // sprintf(buff, "%d %d %d %d\r\n", idx1, idx2, idx3, len);
    // print(buff);
    //!! END Debug -------------------------------------

    if (idx1 < 0 || idx2 < 0 || idx3 < 0 || idx2 <= idx1 || idx3 <= idx2)
    {
        //!! Wronf format!
        print("The "); print(line); print(" is not supported!\r\n");
        return NULL;
    }

    int id = get_id_value( line, idx1, idx2 );
    int val = get_id_value( line, idx2, idx3 );

    //sprintf(buff, "Id:%d Val:%d\r\n", id, val);
    //print(buff);

    //!!
    //!! LEDs
    //!!
    if( 0 == str_index_of_first_token( line, "led" ) )
    {
        if(val == 0) 
        {   //!! OFF
            LED_Off(id);
        }
        else if( val == 1)
        {   //!! ON
            LED_On(id);
        }
        else if( val == 2)
        {   //!! TOGGLE
            LED_Inv(id);
        }
        char *pPage = malloc(150);
        const char *status = LED_Get(id) ? "ON " : "OFF";
        const char *class  = LED_Get(id) ? "danger" : "warning";
        sprintf(pPage, "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nConnection:close\r\n\r\n<h1 class='text-%s'>LED%d: %s</h1>", class, id, status);
        return ( pPage );
        //return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nServer: ECC-Lab\r\nConnection: close\r\n\r\n<h1>THIS IS H1 TEXT</h1>\r\n";
    }
    //!!
    //!! ADCs
    //!!
    else if( 0 == str_index_of_first_token( line, "adc" ) )
    {
        char *pPage = malloc(256);
        int adc = ADC_Get(id);
        sprintf(pPage, "HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nConnection:close\r\n\r\n<h1 class='text-warning'>ADC%d: %.4d</h1>", id, adc);
        return ( pPage );
    }
    return NULL;
}


void ClientRequested(void *evt)
{
    client_t *client = ((server_t *)evt)->client;
    Beep(50);
    client->data = Command_Executor(client->getBuffer);

}

int main(void) 
{

    OS_Init();   

    Beep_PowerSet(0.1);   

    LED_ModeSet( LED_ID_3, LED_MODE_PWM );
    LED_PwmSet( LED_ID_3, 100, 0, 2 );

    Uart1_WriteString("\r\nHTTP Server Application!\r\n"); 



    uint8_t mode;
    if( PSW1_Get() ) {
        mode = 1;  
    }
    else if( PSW2_Get() ) {
        mode = 2;  
    }
    else if( PSW3_Get() ) {
        mode = 3;    
    }
    else{
        mode = 3;     
    }

    WiFi_Init( SSID, PASS, mode, ClientRequested );
    WiFi_InitSoftAP( AP_SSID, AP_PASS, AP_IPADDR, AP_CHANNEL, AP_ENCRYPT );

    OS_Start();                                
}
