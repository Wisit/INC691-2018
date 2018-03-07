
#include "os.h"

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
        memset(ptr, 0, len + 1);
        strcpy(ptr, comd);
    }
    else {
        ptr = (char *)comd;
    }

    cmd.pptr[cmd.put] = ptr;
    cmd.put = (cmd.put + 1) % COMMAND_QUEUE_LENGTH;
    cmd.cnt++;


    char buff[32];
    sprintf(buff, "\r\naddr:%.4X %.4X\r\n", (uint16_t)cmd.pptr[cmd.put-1], (uint16_t)ptr);
    Uart1_AsyncWriteString(buff);
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




const char *AtPositiveKeywords[] = {
    "ready\r\n",
    "OK\r\n",
    "SEND OK\r\n"
};

typedef struct {
    uint8_t    state;
    const char *pComd;
    uint8_t    curSec;
    uint8_t    prvSec;
    uint8_t    timeoutTicks;
}at_command_t;
at_command_t at;

uint8_t AT_StateMachines(const char *line) {
    at.curSec = OS_TimeGet().ss;
    if( at.prvSec != at.curSec ) {

        //!! 1 second
        at.prvSec = at.curSec; 
        
        //!! Timeout checking
        if(++at.timeoutTicks > 5) {
            at.timeoutTicks = 0;
        }   
    }

    return at.state;
}


void Test(void *evt) {
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
}

int main(void) 
{
    //!! Initial the OS
    OS_Init();
    Command_Init();

    OS_TimerCreate("", 1000, 1, Test);
    


    //!! Start OS
    OS_Start();
}


