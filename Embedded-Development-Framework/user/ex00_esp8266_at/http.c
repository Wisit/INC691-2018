#include "http.h"




//!!--------------------------------------------------------------

//!! Homepage, 2003 bytes (can be up to 2048 bytes)
const char * homeHtml = "<html><head><title></title><link rel='icon'href='data:;base64,iVBORw0KGgo='><link rel='stylesheet'href='http://ecc-lab.com/shared/bootstrap.css'><script src='http://ecc-lab.com/shared/jquery.js'></script><script src='http://ecc-lab.com/shared/bootstrap.js'></script><link rel='stylesheet'type='text/css'href='app.css'><script src='app.js'></script></head><body><h1>It Works!</h1><div class'row'><div class'col col-xs-12'><button class='btn btn-primary'>Primary</button><button class='btn btn-success'>Success</button><button class='btn btn-warning'>Warning</button><button class='btn btn-danger'>Danger</button><button class='btn btn-default'>Default</button></div></div><div class'row'><div class'col col-xs-12'><button class='btn btn-primary'>Primary</button><button class='btn btn-success'>Success</button><button class='btn btn-warning'>Warning</button><button class='btn btn-danger'>Danger</button><button class='btn btn-default'>Default</button></div></div><div class'row'><div class'col col-xs-12'><button class='btn btn-primary'>Primary</button><button class='btn btn-success'>Success</button><button class='btn btn-warning'>Warning</button><button class='btn btn-danger'>Danger</button><button class='btn btn-default'>Default</button></div></div><div class'row'><div class'col col-xs-12'><button class='btn btn-primary'>Primary</button><button class='btn btn-success'>Success</button><button class='btn btn-warning'>Warning</button><button class='btn btn-danger'>Danger</button><button class='btn btn-default'>Default</button></div></div><div class'row'><div class'col col-xs-12'><button class='btn btn-primary'>Primary</button><button class='btn btn-success'>Success</button><button class='btn btn-warning'>Warning</button><button class='btn btn-danger'>Danger</button><button class='btn btn-default'>Default</button></div></div><div class'row'><div class'col col-xs-12'><button class='btn btn-primary'>Primary</button><button class='btn btn-success'>Success</button></div></div></body></html>";

//!! CSS
const char * appCss = "h1{color:red;font-size:50px;}";

//!! JS
const char * appJs  = "setInterval(function(){console.log('Hello');},1000);";

//!! favicon.ico
const char * faviconIco = "ECC-Lab";

//!!--------------------------------------------------------------


//!!
//!! Server object
//!!
static server_t server;


//!!
//!! Client object
//!!
static client_t clients[NUM_CLIENTS];


//!! Initialise all parameters of clients
void Client_Init(void) 
{
    uint8_t i;
    client_t *client = clients;
    for(i=0; i<NUM_CLIENTS; i++)
    {
        client->channel      = 0xFF; //!! Connected channel [0, 4]
        client->getBuffer[0] = 0;    //!! Get buffer, 32 bytes maximum
        client->milliSeconds = 0;    //!! Milliseconds, used for multiclientle frames sending
        client->restFrames   = 0;    //!! Number of frames needed to be sent
        client->restBytes    = 0;    //!! Number of bytes needed to be sent
        client++;                    //!! Point to next client object
    }
} //!! Client_Init





//!! This service can be called by LineReceived and Timer (server.timer)
void Server_Service(void *evt) 
{
    //!! Buffer for sprintf()
    char buff[48];

    //!!
    //!! Search for a new client
    //!!
    if( server.client == NULL)
    {
        uint8_t   i;
        for( i=0; i<NUM_CLIENTS; i++ )
        {
            if( clients[i].state == CLIENT_STATE_REQUESTED )
            {
                //!! A new target client is found
                server.client = &clients[i];    
                break;
            }
        } 
    }

     //!! No requested client, return
    if( server.client == NULL ) 
    {
        return;
    }

    //!! Take current client
    client_t *client = server.client;

    //!! Take received line
    const char *line = (char *)evt;

    //!!
    //!! State machines of the server
    //!!
    if( client->state == CLIENT_STATE_REQUESTED ) 
    {
        //!!
        //!! Check requested target
        //!!

        //!! homepage (index.html or empty)
        if( !strcmp(client->getBuffer, "") || !strcmp(client->getBuffer, "index.html")  )
        {
            client->data = homeHtml;
        }
        //!! Get the app.css
        else if( !strcmp(client->getBuffer, "app.css") ) 
        {
            client->data = appCss;
        }
        //!! Get the app.js
        else if( !strcmp(client->getBuffer, "app.js") ) 
        {
            client->data = appJs;
        }
        //!!Get the favicon.ico
        else if( !strcmp(client->getBuffer, "favicon.ico") )
        {
            client->data = faviconIco;
        }

        //!! Other commands
        else 
        {
            //!!
            //!! User's GET commands
            //!!

            //!! Set the client to NULL
            client->data = NULL;

            //!! Call the callback function
            if(server.callback != NULL)
            {
                server.callback(&server);
                //!! In the callback function, the server->client->data = "??" can be applied
            }

            //!! If the server->client->data == NULL, send back the Page Not Found!
            if(client->data == NULL) 
            {
                client->data = "<html><body><h1 style='color:red;'>Page Not Found!</h1></body></html>";  
                
                //!! The client->data can point to a constant staring like this:
                //!!    client->data = POINTER_TO_STRING;
            }
        }

        //!! Check the maximum payload size, 2048 byte
        if( strlen( client->data ) > 2048 ) 
        {
            //!! The client->data is too long (>2048 bytes), create information page
            client->data = "<html><body><h1 style='color:red;'>The payload is greater than 2048 bytes!</h1></body></html>";  
        }

        //!!
        //!! Change client's state and construct the CIPSEND
        //!!
        client->state = CLIENT_STATE_WAIT_OK;
        sprintf( buff, "AT+CIPSEND=%d,%d\r\n", client->channel, strlen( client->data ) );
        AT_Put( buff );
    } 
    //!! Waiting for OK 
    else if( client->state == CLIENT_STATE_WAIT_OK ) 
    {
        //!! The CIPSEND is sent, now waiting for OK
        if( !strcmp(line, "OK\r\n") )
        {
            //!! The OK is detected, send the DATA
            client->state = CLIENT_STATE_WAIT_SEND_OK; 

            //!!--------------------------------------------------
            if(strlen( client->data ) <= 500) 
            {   
                //!! Payload data is quite small, 
                //!! we can put it into the dynamic/heap memory by using the AT_Put()
                //!! The AT_Put() will allocate new block of memory and copy the client->data
                //!! into the allocated memory. The memory will be freed after all bytes are sent
                //!! Noted: the os is compiled with 1024 bytes of heap memory and also the
                //!! Uart2 buffer and Uart2 rung buffer are 1024 bytes, see in config.h
                AT_Put( client->data );
            }
            else 
            {   
                //!! In case of the payload, the client->data, is greater than 500 bytes,
                //!! the payload is devided into some shorter frames, 500 bytes per frame in this case
                //!! To make sure the space of heap memory (1024 bytes) and transmission buffers (Tx, and Ring)
                //!! can be used to storage the frames of data, each frame must be sent separatly, see the block
                //!! of code below how it divided, and check in next state how they are sent
                //!!--------------------------------------------------
                uint16_t length      = strlen(client->data);
                client->restFrames   = length / 500;
                client->restBytes    = length % 500;
                client->milliSeconds = OS_TimeMilliseconds();

            }
        }
    }
    else if( client->state == CLIENT_STATE_WAIT_SEND_OK ) 
    {
        //!! The DATA (frames) are sending, waiting for SEND_OK
        if( !strcmp(line, "SEND OK\r\n") ) 
        {
            //!! The SEND_OK is received, send CIPCLOSE
            client->state = CLIENT_STATE_WAIT_CLOSE_OK;
            sprintf( buff, "AT+CIPCLOSE=%d\r\n", client->channel );
            AT_Put( buff );
        }
        else
        {   //!! Is it more frame to be sent?
            if(client->restFrames) 
            {
                //!! Be sure the previous frame is sent by waiting for 50 mS
                uint32_t currentMs = OS_TimeMilliseconds();
                if(currentMs - client->milliSeconds > 50) 
                {
                    //!! Update the time
                    client->milliSeconds = currentMs;

                    //!! Send the target frame. Note that, do not use the AT_Put()
                    //!! because the OS 's heap memory and the transmisstion buffers
                    //!! (U2TX and U2-Ring buffers) have 1024 bytes.
                    Uart2_AsyncWriteBytes(  (uint8_t *)client->data, 500 ); 
                    client->data += 500;    //!! Point to next frame
                    client->restFrames--;   //!! Decrese the frame counter
                }   
            }
            else 
            {
                //!! No more full frames (500-byte frames), but some bytes are need to be sent 
                //!! After the rest bytes are sent, the module will return OK, and the block
                //!! of code will be executed, state is changed and the CIPCLOSE command will be applied
                Uart2_AsyncWriteBytes( (uint8_t *)client->data, client->restBytes );         
            }
        }
    }
    //!! The CIPCLOSE command is applied, waiting for OK
    else if( client->state == CLIENT_STATE_WAIT_CLOSE_OK ) 
    {
        if( !strcmp(line, "OK\r\n") )
        {
            //!! The OK is received, kill the client

            //!!
            //!! Well done!
            //!!
            os_time_t time = OS_TimeGet();
            sprintf(buff, "%.2d:%.2d:%.2d:%.3d: ", time.hh, time.mm, time.ss, time.ms);
            Uart1_AsyncWriteString(buff);
            Uart1_AsyncWriteString("Completed >> ");
            sprintf(buff, "CH[%d]\r\n", client->channel);
            Uart1_AsyncWriteString(buff);

            //!! Reset
            client->state = CLIENT_STATE_DISCONNECTED; 
            client->channel = 0xFF;
            server.client = NULL;
        }
    }
    else
    {
        //!!
        //!! Something went wrong!, beep for 1 second
        //!!
        Beep(1000);
    }
}



void Http_Service( void *evt )
{


    char buff[48];

    const char *line = (const char *)evt;

    //!! Called by Timer
    if( line == NULL ) 
    {  
        return;
    }

    //!! Called by LineReceived
    else if( 0 == str_index_of_first_token(line, "+IPD,") ) 
    {
        //!! Client GET requested
        //!!
        //!! Format: +IPD,0,419:GET / HTTP/1.1
        //!!
        uint8_t channel = line[5] - 0x30;
        uint8_t idx1 = str_index_of_first_token( line, "GET /" );
        uint8_t idx2 = str_index_of_first_token( line, "HTTP/" );
        if( idx1 < 0 || idx2 < 0 || channel < 0 || channel > 4 ) 
        {
            //!! Wrong Format!!
            sprintf( buff, "\r\nWrong Format! idx1:%d idx2:%d channel:%d\r\n", idx1, idx2, channel );
            Uart1_AsyncWriteString( buff );
            return;
        }

        //!! Take the GET
        char getBuff[CLIENT_GET_BUFFER_LENGTH + 1];
        idx1 += 5; idx2 -= 1;
        uint8_t nb = idx2 - idx1;
        if( nb >= CLIENT_GET_BUFFER_LENGTH ) 
        {
            nb = CLIENT_GET_BUFFER_LENGTH;
        }
        memset(getBuff, 0, CLIENT_GET_BUFFER_LENGTH);   //!! clear
        memcpy( getBuff, (line + idx1), nb );           //!! copy
        //!!------------------------------------------------------------------

        client_t *c = &clients[channel];
        if(c->state == CLIENT_STATE_DISCONNECTED)
        {
            c->state   = CLIENT_STATE_REQUESTED;
            c->channel = channel;
            memset(c->getBuffer, 0, CLIENT_GET_BUFFER_LENGTH);
            strcpy(c->getBuffer, getBuff);
        }
        //!!------------------------------------------------------------------

        os_time_t time = OS_TimeGet();
        sprintf(buff, "%.2d:%.2d:%.2d:%.3d: ", time.hh, time.mm, time.ss, time.ms);
        Uart1_AsyncWriteString(buff);
        Uart1_AsyncWriteString("Requested >> ");
        sprintf(buff, "CH[%d], GET[%s]\r\n", channel, getBuff);
        Uart1_AsyncWriteString(buff);     
    }
}