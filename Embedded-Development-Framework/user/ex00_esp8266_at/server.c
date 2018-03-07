

#include "server.h"




const char * homeHtml = "<html><head><title>Server</title><link rel='stylesheet' type='text/css' href='./app.css'/> <script src='./app.js'></script></head><body><h1>hello world</h1></body></html>";

//!! 1 buttons
//const char * homeHtml = "<html><head><title>Hello</title><link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'><script src='https://code.jquery.com/jquery-3.3.1.slim.min.js'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script><link rel='stylesheet' type='text/css' href='app.css'><script src='app.js'></script></head><body><h1>jQuery and Bootstrap</h1><button class='btn btn-primary'>Primary</button></body></html>";

//!! 2 buttons
//const char * homeHtml = "<html><head><title>Server</title><link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'><script src='https://code.jquery.com/jquery-3.3.1.slim.min.js'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script><link rel='stylesheet' type='text/css' href='app.css'><script src='app.js'></script></head><body><h1>Test jQuery and Bootstrap</h1><div class='container'><div class'row'><div class'col col-xs-12'><button class='btn btn-primary'>Primary</button><button class='btn btn-success'>Success</button></div></div></div></body></html>";


//!! 3 buttons
//const char * homeHtml = "<html><head><title>Server</title><link rel='stylesheet'href='http://ecc-lab.com/shared/bootstrap.css'><script src='http://ecc-lab.com/shared/jquery.js'></script><script src='http://ecc-lab.com/shared/bootstrap.js'></script><link rel='stylesheet'type='text/css'href='app.css'><script src='app.js'></script></head><body><h1>ecc-pic24-cli</h1><div class'row'><div class'col col-xs-12'><button class='btn btn-primary'>Primary</button><button class='btn btn-success'>Success</button><button class='btn btn-danger'>Danger</button></div></div></body></html>";


const char * appCss = "h1{color:red;font-size:50px;}";
const char * appJs  = "setInterval(function(){console.log('Hello');},1000);";



//!!
//!! Server object
//!!
server_t server;



uint8_t activeChannels[5] = {0};


void Looper(void *evt) 
{
    AT_StateMachines();
}

void HTTP_ServerInit(const char *ssid, const char *pass, server_callback_t callback)
{
    server.ip[0] =  0;
    server.mac[0] = 0;
    server.callback = callback;

    server.ssid = ssid;
    server.pass = pass;

    OS_TimerCreate("", 100, 1, Looper);
}


void Server_ExecuteLine(const char *line) 
{
    //!! Buffer for sprintf();
    char buff[64];

    os_time_t time = OS_TimeGet();

    //!! Got ip address, start the server
    if(!strcmp(line, "WIFI GOT IP\r\n")) 
    {
        //!! Start Server
        AT_Put("AT+CIPMUX=1\r\n"); 
        AT_Put("AT+CIPSERVER=1,80\r\n");

        //!! Ask for IP and MAC addresses
        AT_Put("AT+CIFSR\r\n");

        //!! Clear the IP and MAC addresses
        server.ip[0]  = 0;
        server.mac[0] = 0;
    } 
    else if(!strcmp(line, "WIFI CONNECTED\r\n")) 
    {

    }
    else if(!strcmp(line, "WIFI DISCONNECT\r\n")) 
    {

    }
    else if( (0 == str_index_of_first_token(line, "+CIFSR:STAIP,")) || 
             (0 == str_index_of_first_token(line, "+CIFSR:STAMAC,")) ) 
    {
        //!! +CIFSR:STAIP,"192.168.1.124"
        //!! +CIFSR:STAMAC,"5c:cf:7f:23:e1:08"
        uint8_t idx1 = str_index_of_first_char(line, '"');
        uint8_t idx2 = str_index_of_last_char(line, '"');
        idx1 += 1;
        if(line[10] == 'I') 
        {
            memcpy(server.ip, line + (idx1), (idx2-idx1) );
        }
        else if(line[10] == 'M') {
            memcpy(server.mac, line + (idx1), (idx2-idx1) );  
        }
        if(server.ip[0] && server.mac[0]) 
        {
            Beep(20);
            Uart1_AsyncWriteString("\r\nServer is running...\r\n");
            sprintf(buff, "  IP:  %s\r\n", server.ip);
            Uart1_AsyncWriteString(buff);
            sprintf(buff, "  MAC: %s\r\n\n\n", server.mac);
            Uart1_AsyncWriteString(buff);

            if(server.callback != NULL) {
                server.callback(&server);
            }
        }
    }
    else if( 1 == str_index_of_first_token(line, ",CLOSED") ) 
    {
        Uart1_AsyncWriteString(line);

        activeChannels[line[1]-0x30] = 0;
    }
    else if( 1 == str_index_of_first_token(line, ",CONNECT") ) 
    {
        Uart1_AsyncWriteString(line);

        activeChannels[line[1]-0x30] = 1;
    }
    else if( 0 == str_index_of_first_token(line, "+IPD,") ) 
    {
        //!! Client GET requested
        Uart1_AsyncWriteString(line);

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
            return;
        }

        //!! Take the GET
        char getBuff[32];
        idx1 += 5; idx2 -= 1;
        memset( getBuff, 0, 32 );
        memcpy( getBuff, (line + idx1), (idx2 - idx1) );


        if(activeChannels[channel]) {
            sprintf( buff, "AT+CIPCLOSE=%d\r\n", channel );
            AT_Put( buff );
            return;
        }   

        

        //!!
        //!! Print client requested information
        //!!
        // sprintf( buff, "%.2d:%.2d:%.2d : ", time.hh, time.mm, time.ss );
        // Uart1_AsyncWriteString( buff );
        // sprintf( buff, "client:%d GET:%s\r\n", channel, getBuff );
        // Uart1_AsyncWriteString( buff );

        //!!
        //!! Ignore the favicon.ico
        //!!
        // if( !strcmp( getBuff, "favicon.ico" ) ) {
        //     sprintf( buff, "AT+CIPCLOSE=%d\r\n", channel );
        //     AT_Put( buff );
        //     return;
        // }

        //!!
        //!!
        //!!
        //char page[32];
        //sprintf( page, "Time: %.2d:%.2d:%.2d\r\n", time.hh, time.mm, time.ss );
        char * page;
        if( !strcmp(getBuff, "") )
        {
            page = homeHtml;
        }
        else if(!strcmp(getBuff, "app.css")) {
            page = appCss;
        }
        else if(!strcmp(getBuff, "app.js")) {
            page = appJs;
        }
        else {
            page = "HELLO";
        }


        sprintf( buff, "AT+CIPSEND=%d,%d\r\n", channel, strlen(page) );
        Uart1_AsyncWriteString( buff );


        AT_Put( buff );//!! Request to send
        AT_Put( page );//!! Send the data

        sprintf( buff, "AT+CIPCLOSE=%d\r\n", channel );
        AT_Put( buff);//!! Close the connection

        // printf( buff, "AT+CIPSEND=%d,%d\r\n", channel, strlen(page) );
        // Uart2_AsyncWriteString( buff ); __delay_ms(50);
        // Uart2_AsyncWriteString( page ); __delay_ms(500);
        // sprintf( buff, "AT+CIPCLOSE=%d\r\n", channel );
        // Uart2_AsyncWriteString( buff ); __delay_ms(50);
        


    }
}