
#include "os.h"


const char *SSID = "ECC-Mobile";
const char *PASS = "ecclab1122";

#define COMMAND_QUEUE_LENGTH    16
typedef struct {
    char *pptr[COMMAND_QUEUE_LENGTH];
    uint8_t put;
    uint8_t get;
    uint8_t cnt;
    bool isConst;
}command_queue_t;
command_queue_t cmd;

#define _COMMAND_DEBUG_

void Command_Init(void) {
    cmd.put = 0;
    cmd.get = 0;
    cmd.cnt = 0;
}   

bool Command_Put(const char *comd, bool isConst) {
    if(cmd.cnt >= COMMAND_QUEUE_LENGTH) {
        return false;
    }

    char *ptr;
    if(!isConst) {
        uint8_t len = strlen(comd);
        ptr = (char *)malloc(len + 1);
        //memset(ptr, 0, len + 1);
        strcpy(ptr, comd);
    }
    else {
        ptr = (char *)comd;
    }

    cmd.pptr[cmd.put] = ptr;
    cmd.put = (cmd.put + 1) % COMMAND_QUEUE_LENGTH;
    cmd.cnt++;

    #ifdef _COMMAND_DEBUG_
    {
        char buff[32];
        sprintf(buff, "addr:%.4X %.4X %d\r\n", (uint16_t)cmd.pptr[cmd.put-1], (uint16_t)ptr, strlen(comd)+1);
        Uart1_AsyncWriteString(buff);
    }
    #endif //!! _COMMAND_DEBUG_
    return true;
}

bool Command_Get(char **pp) {
    if(cmd.cnt <= 0) {
        return false;
    }
    *pp = cmd.pptr[cmd.get];
    cmd.get = (cmd.get + 1) % COMMAND_QUEUE_LENGTH;
    cmd.cnt--;
    return true;
}



#define AT_RETURN_READY     0    //!! Module ready
#define AT_RETURN_OK        1    //!! Command is executed
#define AT_RETURN_SEND_OK   2    //!! Data is sent
#define AT_RETURN_BUSY      3
#define AT_RETURN_UNKNOWN  0xFF //!! Others
//!! Must be matched to the lines above!
const char *AtKeywords[] = {
    "ready\r\n",
    "OK\r\n",
    "SEND OK\r\n",
    "busy p...\r\n",
    ""              //!! Must be ended with EMPTY string
};


#define AT_STATE_READY              0
#define AT_STATE_WAIT_RESPONSE      1
#define AT_STATE_GOT_RESPONSE       2
#define AT_STATE_WAIT_TIMEOUT       3

typedef struct {
    uint8_t    state;
    const char *pComd;
    uint8_t    curSec;
    uint8_t    prvSec;
    uint8_t    timeoutTicks;
    uint8_t    targetCode;
    uint8_t    returnCode;
}at_command_t;
at_command_t at;


void AT_Init(void) {
    at.state       = AT_STATE_READY;
    at.returnCode  = AT_RETURN_READY;
    at.targetCode  = AT_RETURN_OK;
    at.timeoutTicks = 0;
}

uint8_t AT_ProcessLine(const char *line) {
    uint8_t i = 0;
     at.returnCode = AT_RETURN_UNKNOWN;
    while(strcmp(AtKeywords[i], "")) {
        if(!strcmp(line, AtKeywords[i])) {
            at.state      = AT_STATE_GOT_RESPONSE;
            at.returnCode = i;
            return i;
        }
        i++;
    }

    // if(!strcmp(line, "OK\r\n")) {
    //     at.state      = AT_STATE_GOT_RESPONSE;
    //     at.returnCode = AT_RETURN_OK;
    // }
   
    return at.returnCode;
}

void AT_StateMachines(void) {

    char buff[48];

    at.curSec = OS_TimeGet().ss;

    if( at.prvSec != at.curSec ) {

        //!! 1 second
        at.prvSec = at.curSec; 

        //!! Running in TIMEOUT state, print report
        if( at.state == AT_STATE_WAIT_TIMEOUT ) {
            //!! Report and return
            sprintf(buff, "Timeout: at.state=%d retCode=%d %s\r\n", at.state, at.returnCode, at.pComd);
            Uart1_AsyncWriteString(buff);
        }
        
        //!! Waiting for response, Timeout checking
        else if(at.state == AT_STATE_WAIT_RESPONSE) {
            if( ++at.timeoutTicks >= 5 ) {
                at.timeoutTicks = 0;
                at.state = AT_STATE_WAIT_TIMEOUT;

                //!! Free memory
                free((char *)at.pComd);
                at.pComd = NULL;
            }
        } 
        else 
        {
            sprintf(buff, "Info: at.state=%d at.retCode=%d %s\r\n", at.state, at.returnCode, at.pComd);
            Uart1_AsyncWriteString(buff);
        }
    }

    if(at.state == AT_STATE_GOT_RESPONSE) 
    {

        if(at.state == AT_RETURN_BUSY) {
            return;
        }

        //!! Free memory
        free((char *)at.pComd);
        at.pComd = NULL;

        if(at.returnCode != AT_RETURN_UNKNOWN) 
        {   //!! ready, OK, SEND_OK
            at.returnCode = AT_RETURN_UNKNOWN;
            at.state = AT_STATE_READY;
        }
        else
        {
            return;
        }
    }
    else if(at.state == AT_STATE_WAIT_TIMEOUT) {
        return;
    }

    char *ptr = 0;
    if(Command_Get(&ptr)) 
    {
        at.state = AT_STATE_WAIT_RESPONSE;
        Uart1_AsyncWriteString("\r\n>>AT: ");
        Uart1_AsyncWriteString(ptr);
        Uart2_AsyncWriteString(ptr);
    }

}


void Test1(void *evt) {
    /*
    LED_Flash(LED_ID_0, 10);
    Command_Put("AT\r\n", false);
    Command_Put("AT0\r\n", false);
    Command_Put("AT+GMR\r\n", false);
    Command_Put("AT+CWMODE=1\r\n", false);
    char *ptr = 0;
    Uart1_AsyncWriteString("\r\n");
    while(Command_Get(&ptr)) {
        char buff[32];
        sprintf(buff, "COMMAND: %s", ptr);
        Uart1_AsyncWriteString(buff);
        __delay_ms(100);
        free(ptr);
    }
    Uart1_AsyncWriteString("\r\n");
    */
}


void ESPLineReceived(void *evt) {
    //!! Get received line
    const char *line = (const char *)((uart_event_t *)evt)->data.buffer;
    Uart1_AsyncWriteString("recv: ");
    Uart1_AsyncWriteString(line);
    AT_ProcessLine(line);

    if(!strcmp(line, "WIFI GOT IP\r\n")) {
        
        //Command_Put("AT+CIFSR\r\n", true);
        //__delay_ms(1000);
        
        Command_Put("AT+CIPSERVER=1,80\r\n", true);
        Command_Put("AT+CIFSR\r\n", true);
        
    }
    
    
}

void Test(void *evt) {

    AT_StateMachines();
}
int main(void) 
{
    //!! Initial the OS
    OS_Init();
    Command_Init();
    AT_Init();

    OS_TimerCreate("", 200, 1, Test);

    OS_Uart2SetLineReceivedCallback(ESPLineReceived);

    //!! Setup commands
    Command_Put("AT+RST\r\n", false);
    Command_Put("ATE0\r\n", false);
    //Command_Put("AT+GMR\r\n", true);
    //Command_Put("AT+GMR\r\n", true);
    //Command_Put("AT+GMR\r\n", true);
    Command_Put("AT+CWMODE=1\r\n", true);  
    Command_Put("AT+CIPMUX=1\r\n", true); 
    char buff[48];
    sprintf(buff, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS);
    Command_Put(buff, false);

    // Command_Put("AT+CIPMUX=1\r\n", true);
    // Command_Put("AT+CIPSERVER=1,80\r\n", true);
    // Command_Put("AT+CIFSR\r\n", true);
    
    
    //Command_Put("AT+CIPMUX=1\r\n", true);
    //Command_Put("AT+CIPSERVER=1,80\r\n", true);
    //Command_Put("AT+CIFSR\r\n", true);

    //!! Start OS
    OS_Start();
}


