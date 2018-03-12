
#include "at2.h"


//!! Command and AT objects (Global variables)
command_queue_t  cmd;
at_command_t     at;


//!! Take a block of memory from heap, and copy the command into the located memory
bool AT_Put(const char *pComd) 
{
    if(cmd.cnt >= COMMAND_QUEUE_LENGTH) 
    {
        //!! No more space for the new command
        return false;
    }

    //!! Allocate memory and copy the command
    uint8_t len = strlen(pComd);
    char *ptr = (char *)malloc(len + 1);
    strcpy(ptr, pComd);
    
    //!! Update the command object's variables
    cmd.pptr[cmd.put] = ptr;
    cmd.put = (cmd.put + 1) % COMMAND_QUEUE_LENGTH;
    cmd.cnt++;

    return true;
}

//!!
//!!#define _AT_GET_DEBUG_
//!!
bool AT_Get(char **ppComd) 
{
    if(cmd.cnt <= 0) 
    {
        //!! No more command in the queue
        return false;
    }
    //!! Point to a next command in the queue
    *ppComd = cmd.pptr[cmd.get];


     #ifdef _AT_GET_DEBUG_
        char buff[32];
        sprintf(buff, "\t>>addr:0x%.4X\r\n", (uint16_t)*ppComd);
        Uart1_AsyncWriteString(buff);
    #endif //!! _AT_GET_DEBUG_

    //!! Update command object's variables
    cmd.get = (cmd.get + 1) % COMMAND_QUEUE_LENGTH;
    cmd.cnt--;
    return true;
}

void AT_CleanMemory(void)
{
     //!! Free all blocks of memories
    char *pMem = 0;
    while( AT_Get( &pMem ) )
    {
        free( pMem );
        pMem = NULL;
    }

    cmd.put = 0;
    cmd.get = 0;
    cmd.cnt = 0;
}

void AT_ResetState(void) 
{
    at.state        = AT_STATE_READY;
    at.returnCode   = AT_RETURN_READY;
    at.targetCode   = AT_RETURN_OK;
    at.timeoutTicks = 0;
}

//!! Initialise all variable of the Command and AT objects
void AT_Init(void) 
{
    AT_ResetState();
    AT_CleanMemory();
}


//!!
//#define _AT_FREE_DEBUG_
//!!
void AT_FreeMemory(void) 
{
    if(at.pComd == NULL) 
    {
        return;
    }

    #ifdef _AT_FREE_DEBUG_
        char buff[32];
        sprintf(buff, "\t>>free:0x%.4X\r\n", (uint16_t)at.pComd);
        Uart1_AsyncWriteString(buff);
    #endif //!! _AT_FREE_DEBUG_

    //!! Free the memory pointed by the at.pComd
    free(at.pComd);
    at.pComd = NULL;
}


#define AT_ApplyStateChanged(newState){ \
    at.state = newState;                \
    if(at.callback != NULL){            \
        at.callback(&at);               \
    }                                   \
}


void AT_Service(void) 
{

    static uint16_t prv_sec = 0;
    uint16_t cur_sec = OS_TimeGet().ss;
    if(prv_sec != cur_sec)
    {   
        prv_sec = cur_sec;
        
        if(at.state == AT_STATE_WAIT_RESPONSE)
        {
            Uart1_AsyncWriteString("AT-Service: Waiting for response...\r\n");
            if(++ at.timeoutTicks > 5)
             {
                at.timeoutTicks = 0;
                Beep(10);

                AT_CleanMemory();

                at.state = AT_STATE_READY;
            }
            return;
        }
        else if(at.state == AT_STATE_READY)
        {
            //Uart1_AsyncWriteString("AT-SRV: Waiting for new command...\r\n");
        }
        else if(at.state == AT_STATE_WAIT_TIMEOUT)
        {
            Uart1_AsyncWriteString("AT-Service: Timeout!!\r\n");
        }
        else if(at.state == AT_STATE_GOT_RESPONSE)
        {
            Uart1_AsyncWriteString("AT-Service: Already got response!\r\n");
        }
        else
        {
            Uart1_AsyncWriteString("AT-Service: Unknown state!!\r\n");
        }
    }


    //!!-------------------------------------------------------
    if( at.state == AT_STATE_GOT_RESPONSE )
    {
        AT_FreeMemory();

        if(at.returnCode == AT_RETURN_OK)
        {
            //!! Command is executed
            //Uart1_AsyncWriteString("AT-RET: AT_RETURN_OK\r\n\r\n");
            at.state = AT_STATE_READY;  //!! Ready for new command
        }
        else if(at.returnCode == AT_RETURN_SEND_OK)
        {
            //!! Data is sent
            //Uart1_AsyncWriteString("AT-RET: AT_RETURN_SEND_OK\r\n\r\n");
            at.state = AT_STATE_READY;  //!! Ready for new command
        }
        else if(at.returnCode == AT_RETURN_READY)
        {
            //!! Restarted
            //Uart1_AsyncWriteString("AT-RET: AT_RETURN_READY\r\n\r\n");
            at.state = AT_STATE_READY;  //!! Ready for new command
        }
        else if(at.returnCode == AT_RETURN_BUSY)
        {
            //!! In progress...
            //Uart1_AsyncWriteString("AT-RET: AT_RETURN_BUSY\r\n\r\n");
        }
        else if(at.returnCode == AT_RETURN_ERROR)
        {
            //!! Something wrong!
            //Uart1_AsyncWriteString("\r\n\r\nAT-RET: AT_RETURN_ERROR\r\n\r\n");
            
            at.state = AT_STATE_READY;
            AT_Put( "AT+RST\r\n" );
        }
        else if(at.returnCode == AT_RETURN_FAIL)
        {
            //!! Something wrong!
            //Uart1_AsyncWriteString("\r\n\r\nAT-RET: AT_RETURN_FAIL\r\n\r\n");
        }

        if(at.state == AT_STATE_READY)
        {
            //Uart1_AsyncWriteString("AT-SRV: Waiting for new command...\r\n");
        }
    }

    //!!-------------------------------------------------------
    if(at.state == AT_STATE_READY)
    {

        char *ptr = 0;
        if( AT_Get( &ptr ) ) 
        {
            //Uart1_AsyncWriteString("\r\nAT-WRT: ");
            //Uart1_AsyncWriteString(ptr);

            AT_FreeMemory();

            at.pComd = ptr;                     //!! Load current command
            at.state = AT_STATE_WAIT_RESPONSE;  //!! Change state
            at.returnCode = AT_RETURN_UNKNOWN;  //!! Initial with Unknown
            Uart2_AsyncWriteString(ptr);        //!! Write to ESP
        }
    }
}

void AT_ProcessLine(const char *line)
{

    //!!-------------------------------------------------------
    if( 0 == str_index_of_first_token(line, "OK\r\n") )
    {
        at.state = AT_STATE_GOT_RESPONSE;
        at.returnCode = AT_RETURN_OK;
    }
    else if( 0 == str_index_of_first_token(line, "SEND OK\r\n") )
    {
        at.state = AT_STATE_GOT_RESPONSE;
        at.returnCode = AT_RETURN_SEND_OK;
    }
    else if( 0 == str_index_of_first_token(line, "ready\r\n") )
    {
        at.state = AT_STATE_GOT_RESPONSE;
        at.returnCode = AT_RETURN_READY;
    }
    else if( 0 == str_index_of_first_token(line, "busy p...\r\n") ||  0 == str_index_of_first_token(line, "busy s...\r\n") )
    {
        at.state = AT_STATE_GOT_RESPONSE;
        at.returnCode = AT_RETURN_BUSY;
    }
    else if( 0 == str_index_of_first_token(line, "ERROR\r\n") )
    {
        at.state = AT_STATE_GOT_RESPONSE;
        at.returnCode = AT_RETURN_ERROR;
    }
    else if( 0 == str_index_of_first_token(line, "FAIL\r\n") )
    {
        at.state = AT_STATE_GOT_RESPONSE;
        at.returnCode = AT_RETURN_FAIL;
    }
    else 
    {
        //Uart1_AsyncWriteString("\tReceived line: ");
        //Uart1_AsyncWriteString(line);
        return;
    }
}
