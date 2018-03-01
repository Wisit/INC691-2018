#include "at.h"

//!! Uncomment this line to print system information
// #define __DEBUG_AT__

//!! Global object
at_command_t atComd;

const char *AtStateMsg[] = {
    "[0] AT_STATE_WAIT_COMMAND  ",
    "[1] AT_STATE_WAIT_RESPONSE ",
    "[2] AT_STATE_GOT_RESPONSE  ",
    "[3] AT_STATE_WAIT_TIMEOUT  "};

//!!
void AT_CommandInit(os_callback_t callback)
{
    atComd.put = 0;
    atComd.get = 0;
    atComd.cnt = 0;
    atComd.state = AT_STATE_WAIT_COMMAND;
    atComd.timeoutCallback = callback;
}

//!!
void AT_PutCommand(char *command)
{
    at_command_t *p = &atComd;
    uint8_t clen = strlen(command) + 1;
    char *pc = (char *)malloc(clen);
    memset(pc, 0, clen);
    strcpy(pc, command);
    *(p->pComds + p->put) = pc;
    p->put = (p->put + 1) % AT_QUEUE_LENGTH;
    p->cnt++;

    #ifdef __DEBUG_AT__
    char buff[48];
    sprintf(buff, "AT.memory:   0x%.4X 0x%.4X\r\n", (uint16_t)pc, (uint16_t) * (p->pComds + 0));
    Uart1_AsyncWriteString(buff);
    #endif //!! __DEBUG_AT__

}

//!!
bool AT_FetchCommand(void)
{
    at_command_t *p = &atComd;
    if (p->cnt <= 0)
    {
        return false;
    }
    p->currentComd = *(p->pComds + p->get);
    p->get = (p->get + 1) % AT_QUEUE_LENGTH;
    p->cnt--;
    return true;
}

//!!
void AT_FreeMemory(void)
{
    free(atComd.currentComd);
}

//!!
int AT_ProcessLine(const char *line)
{
    if (atComd.state == AT_STATE_WAIT_RESPONSE)
    {
        //!! In this case, only "OK\r\n" is checked
        if (!strcmp(line, "OK\r\n"))
        {
            AT_FreeMemory();
            atComd.state = AT_STATE_GOT_RESPONSE;
            atComd.state = AT_STATE_WAIT_COMMAND;
        }
        else if (!strcmp(line, "ready\r\n"))
        {

        }
    }
    return atComd.state;
}

//-----------------------------------------------------------------------------------
#ifdef __DEBUG_AT__
#define at_print_debug()                                                          \
{                                                                                 \
        char *buff = (char *)malloc(48);                                          \
        sprintf(buff, "AT.state:  %s [%d] ", AtStateMsg[atComd.state], waitingSec); \
        Uart1_AsyncWriteString(buff);                                             \
        sprintf(buff, "AT.queue: %d\r\n", atComd.cnt);                            \
        Uart1_AsyncWriteString(buff);                                             \
        sprintf(buff, "AT.memory: 0x%.4X\r\n", (uint16_t)buff);                   \
        Uart1_AsyncWriteString(buff);                                             \
        free(buff);                                                               \
}
#endif //!! __DEBUG_AT__
//-----------------------------------------------------------------------------------

//!! Must be called as a callback of timer
void AT_Loop(void *evt)
{
    static uint8_t prev_sec = 0;
    static uint16_t waitingSec = 0;
    os_time_t time = OS_TimeGet();

    if (atComd.state == AT_STATE_WAIT_COMMAND)
    {
        //!! Fetch a command in the command-queue
        if (AT_FetchCommand())
        {
            //!! Sent the command to ESP8266. The atComd.state will be shanged in
            //! EspLineReceived() or AT_TimeoutCallback()
            atComd.state = AT_STATE_WAIT_RESPONSE;

            
            Uart1_AsyncWriteString("AT Send: ");
            Uart1_AsyncWriteString(atComd.currentComd);
            Uart1_AsyncWriteString("");
            
            Uart2_AsyncWriteString(atComd.currentComd);

            //!! Start the count down timer to check timeout
            //OS_TimerDelete(atComd.timer);
            //atComd.timer = OS_TimerCreate("ATTimeout", 10000, 0, AT_TimeoutCallback);

            //!! Command is inserted in queue, speed up the AT_Loop timer to 100 mS
            timer_t *t = ((event_t *)evt)->sender;
            OS_TimerSetTicks(t, 100);
        }
        else
        {
            //!! No more command in the queeu, slow down the AT_Loop timer to 1 second
            timer_t *t = ((event_t *)evt)->sender;
            OS_TimerSetTicks(t, 1000);
        }
    }
    
    if (time.ss != prev_sec)
    {
        //!! 1 second periodically checkinging
        prev_sec = time.ss;
        if (atComd.state == AT_STATE_WAIT_RESPONSE)
        {
            waitingSec++; 
            if (waitingSec >= 10)
            { //!! 10 seconds timeout! no token, "OK\r\n" is received
                waitingSec = 0;
                atComd.state = AT_STATE_WAIT_TIMEOUT;
                if (atComd.timeoutCallback != NULL)
                {
                    //!! Execute the timeout callback
                    atComd.timeoutCallback((void *)&atComd);

                    //!! Fource the state to AT_STATE_WAIT_COMMAND
                    //atComd.state = AT_STATE_WAIT_COMMAND;
                }
            }
        }
        else {
            waitingSec = 0;
        }

        #ifdef __DEBUG_AT__
        at_print_debug();
        #endif //!!__DEBUG_AT__

        char buff[48];
        sprintf(buff, "%.2d:%.2d:%.2d ", time.hh, time.mm, time.ss);
        Uart1_AsyncWriteString(buff);
        sprintf(buff, "at.state:   %s [%d]\r\n", AtStateMsg[atComd.state], waitingSec);
        Uart1_AsyncWriteString(buff);
    }
}
