/************************************************************
 * File:    esp.c                                           *
 * Author:  Dr.Santi Nuratch                                *
 *          Embedded Computing and Control Laboratory       *
 * Update:  09 March 2018, 06.51 PM                         *
 ************************************************************/

#include "esp.h"

//!! Extern variable
extern at_command_t at;


//!! Global variable
esp_module_t esp;


//!! Sends AT command and wait for 100 mS
//!! If the module returns OK, the esp.state will be updated to ESP_STATE_DETECTED
void ESP_Service( void *evt ) 
{
    const char *line = (const char *)evt;

    //!!
    if( esp.state  == ESP_STATE_NOTFOUND )
    {
        //!! Called bu LineREceived
        if( line != NULL ) 
        { 
            if( !strcmp( line, "OK\r\n" ) || !strcmp( line, "ready\r\n" )) 
            {
                esp.state = ESP_STATE_DETECTED;
                //Uart1_AsyncWriteString( "ESP8266 is detected\r\n" );
            }
            return;
        }
        //!! Called by Timer
        else 
        {   
            //!! Ask for OK response
            if(OS_TimeGet().ss % 2)
            {
                AT_Put( "AT\r\n" );
            }
        }
    }
}

void ESP_Init( os_callback_t callback ) 
{
    esp.state  = ESP_STATE_NOTFOUND;
    esp.pTimer = NULL;
    esp.callback = callback;
    Uart2_ClearRingBuffer();
    OS_Uart2SetLineReceivedCallback( esp.callback );
    //AT_Put( "AT+RST\r\n" );
}
