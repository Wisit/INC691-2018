
#include "at.h"


#define AT_RETURN_READY     0    //!! Module ready
#define AT_RETURN_OK        1    //!! Command is executed
#define AT_RETURN_SEND_OK   2    //!! Data is sent
#define AT_RETURN_BUSY      3
#define AT_RETURN_FAIL      4
#define AT_RETURN_ERROR     5
#define AT_RETURN_UNKNOWN  0xFF //!! Others
//!! Must be matched to the lines above!
const char *AtKeywords[] = {
    "ready\r\n",
    "OK\r\n",
    "SEND OK\r\n",
    "busy p...\r\n",
    "FAIL\r\n",
    "ERROR\r\n",
    ""              //!! Must be ended with EMPTY string
};

static command_queue_t cmd;
static at_command_t at;

//!!#define _AT_PUT_DEBUG_
bool AT_Put(const char *pComd) {
    if(cmd.cnt >= COMMAND_QUEUE_LENGTH) {
        return false;
    }

    char *ptr;
    if(1) {
        uint8_t len = strlen(pComd);
        ptr = (char *)malloc(len + 1);
        strcpy(ptr, pComd);
    }
    else {
        ptr = (char *)pComd;
    }

    cmd.pptr[cmd.put] = ptr;
    cmd.put = (cmd.put + 1) % COMMAND_QUEUE_LENGTH;
    cmd.cnt++;

    #ifdef _AT_PUT_DEBUG_
    {
        char buff[32];
        sprintf(buff, "\t>>addr:0x%.4X 0x%.4X %d\r\n", (uint16_t)cmd.pptr[cmd.put-1], (uint16_t)ptr, strlen(pComd)+1);
        Uart1_AsyncWriteString(buff);
    }
    #endif //!! _AT_PUT_DEBUG_
    return true;
}

bool AT_Get(char **ppComd) {
    if(cmd.cnt <= 0) {
        return false;
    }
    *ppComd = cmd.pptr[cmd.get];
    cmd.get = (cmd.get + 1) % COMMAND_QUEUE_LENGTH;
    cmd.cnt--;
    return true;
}


void AT_Init(void) {

    cmd.put = 0;
    cmd.get = 0;
    cmd.cnt = 0;

    at.state        = AT_STATE_READY;
    at.returnCode   = AT_RETURN_READY;
    at.targetCode   = AT_RETURN_OK;
    at.timeoutTicks = 0;
}

uint8_t AT_ExecuteLine(const char *line) {
    uint8_t i = 0;
    while(strcmp(AtKeywords[i], "")) {
        if(!strcmp(line, AtKeywords[i])) {
            at.state      = AT_STATE_GOT_RESPONSE;
            at.returnCode = i;

            Uart1_AsyncWriteString("\r\n>>Keyword: ");
            Uart1_AsyncWriteString(AtKeywords[i]);

            return i;
        }
        i++;
    }
    return at.returnCode;
}

//!!#define _AT_FREE_DEBUG_
void AT_FreeMemory(void) {
    #ifdef _AT_FREE_DEBUG_
        char buff[32];
        sprintf(buff, "\t>>free:0x%.4X\r\n", (uint16_t)at.pComd);
        Uart1_AsyncWriteString(buff);
    #endif //!! _AT_FREE_DEBUG_
    free(at.pComd);
    at.pComd = NULL;
}


//!!#define _AT_STATE_INFO_DEBUG_
//!!#define _AT_STATE_BUSY_DEBUG_
//!!#define _AT_STATE_PUT_DEBUG_

void AT_StateMachines(void) 
{

    //!! Buffer for sprintf();
    char buff[48];

    //!! Get current time
    at.curSec = OS_TimeGet().ss; 

    //!! Check 1 second
    if( at.prvSec != at.curSec ) 
    {

        //!! Update the second
        at.prvSec = at.curSec; 

        //!! Running in TIMEOUT state, print report
        if( at.state == AT_STATE_WAIT_TIMEOUT ) 
        {
            //!! Print the timeout ant atate information
            sprintf(buff, "\r\n\r\nTimeout: at.state=%d retCode=%d\r\n", at.state, at.returnCode);
            Uart1_AsyncWriteString(buff);

            //
            //!! Timeout!!! WHAT TO DO?
            //
            AT_Put("AT+RST\r\n");
            at.state = AT_STATE_READY;
        }
        
        //!! Waiting for response, Timeout checking
        else if(at.state == AT_STATE_WAIT_RESPONSE) 
        {
            if( ++at.timeoutTicks >= 5 ) 
            {
                //!! Timeout!
                at.timeoutTicks = 0;
                at.state = AT_STATE_WAIT_TIMEOUT;

                //!! Free memory
                AT_FreeMemory();
            }
        } 
        else 
        {
            #ifdef _AT_STATE_INFO_DEBUG_
                sprintf(buff, "Info: at.state=%d at.retCode=%d %s\r\n", at.state, at.returnCode, at.pComd);
                Uart1_AsyncWriteString(buff);
            #endif //!! _AT_STATE_INFO_DEBUG_
        }
    }
    //!!
    //!! Is the AT_STATE_GOT_RESPONSE received?
    //!!
    if(at.state == AT_STATE_GOT_RESPONSE) 
    {
        //!! Got a response message
        if(at.returnCode == AT_RETURN_BUSY) 
        {
            //!! Response with busy, return and wait for nexr turn
            #ifdef _AT_STATE_BUSY_DEBUG_
                Uart1_AsyncWriteString("\tat.state AT_RETURN_BUSY\r\n");
            #endif //!! _AT_STATE_BUSY_DEBUG_
            
            return;
        }

        //!! Free memory
        AT_FreeMemory();

        //!!
        //!! ERROR
        //!!
        if(at.returnCode == AT_RETURN_ERROR) 
        {
            at.returnCode = AT_RETURN_UNKNOWN;
            at.state = AT_STATE_READY;

            sprintf(buff, "\r\n\r\n!!Error! Restarting...\r\n\r\n");
            Uart1_AsyncWriteString(buff);

            AT_Put("AT+RST\r\n");
            return;
        }
        //!!
        //!! FAIL
        //!!
        if(at.returnCode == AT_RETURN_FAIL) 
        {
            at.returnCode = AT_RETURN_UNKNOWN;
            at.state = AT_STATE_READY;

            sprintf(buff, "\r\n\r\n!!Fail! Restarting...\r\n\r\n");
            Uart1_AsyncWriteString(buff);

            AT_Put("AT+RST\r\n");
            return;
        }


        //!!
        //!! "ready",  "OK", "SEND_OK", "busy p..."
        //!!
        if(at.returnCode != AT_RETURN_UNKNOWN) 
        {   //!! ready, OK, SEND_OK, busy p...
            at.returnCode = AT_RETURN_UNKNOWN;
            at.state = AT_STATE_READY;     
        }
        else
        {
            //!! Undefined code, return
            return;
        }
    }
    //!! It's in time out state, return
    else if(at.state == AT_STATE_WAIT_TIMEOUT) 
    {
        return;
    }

    //!!
    //!! Ready for new command/data
    //!!
    char *ptr = 0;
    if(AT_Get(&ptr)) 
    {
        //!! Save the memory address for free() later
        at.pComd = ptr;

        //!! Change the at.atate to WAIT_RESPONSE
        at.state = AT_STATE_WAIT_RESPONSE;

        //!! Send command/data to the module
        Uart2_AsyncWriteString(ptr);

        #ifdef _AT_STATE_PUT_DEBUG_
            Uart1_AsyncWriteString("\r\n>>AT: ");
            Uart1_AsyncWriteString(ptr);
        #endif
    }

}//!! AT_StateMachines

