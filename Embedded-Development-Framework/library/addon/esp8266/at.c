#include "at.h"


//!! Global object
at_command_t at;

//!! AT debugging messages
const char *AtStateMessage[] = {
    "[0] AT_STATE_MODULE_DISCONNECTED",
    "[1] AT_STATE_MODULE_CONNECTEED",
    "[2] AT_STATE_READY_FOR_COMMAND",
    "[3] AT_STATE_WAIT_OK_RESPONSE",
    "[4] AT_STATE_GOT_OK_RESPONSE",
    "[5] AT_STATE_WAIT_TIMEOUT"};

//!! Initialse the object, the at
void AT_CommandInit(os_callback_t stateChangedCallback){
    at.put = 0;
    at.get = 0;
    at.cnt = 0;
    at.state = AT_STATE_READY_FOR_COMMAND;
    at.stateChangedCallback = stateChangedCallback;
}

//!! Put a command into queue
bool AT_PutCommand(char *command){
    at_command_t *p = &at;
    if (p->cnt >= AT_QUEUE_LENGTH) {
        //!! No space for this command
        return false;
    }
    uint8_t clen = strlen(command) + 1;
    char *pc = (char *)malloc(clen);
    memset(pc, 0, clen);
    strcpy(pc, command);
    *(p->pComds + p->put) = pc;
    p->put = (p->put + 1) % AT_QUEUE_LENGTH;
    p->cnt++;

    //!! Memory address checking
    #ifdef __DEBUG_AT__
    char buff[48];
    sprintf(buff, "AT.memory:   0x%.4X 0x%.4X\r\n", (uint16_t)pc, (uint16_t) * (p->pComds + 0));
    Uart1_AsyncWriteString(buff);
    #endif //!! __DEBUG_AT__

    return true;
}

//!! Get a command from queue, just point to the memory of the next command
bool AT_FetchCommand(void) {
    at_command_t *p = &at;
    if (p->cnt <= 0)  {
        //!! No more command in queue
        return false;
    }
    p->currentComd = *(p->pComds + p->get);
    p->get = (p->get + 1) % AT_QUEUE_LENGTH;
    p->cnt--;
    return true;
}

//!! Free memory. Must be call this function after the command is sent
void AT_FreeMemory(void) {
    free(at.currentComd);
}

//!! Change the at.atate and execute the callback function
#define AT_ApplyStateChange(ns) {               \
    at.state = ns;                              \
    if(at.stateChangedCallback != NULL) {       \
        at.stateChangedCallback((void *)&at);   \
    }                                           \
}

//!! This callback function must be called by LineReceived callbck of the UART2/ESP8266
uint16_t AT_ProcessLine(const char *line) {
    //!! The command is applied and the at.state is changed to AT_STATE_GOT_OK_RESPONSE somewhere
    //!! Some tokens are needed to be checked here
    if (at.state == AT_STATE_WAIT_OK_RESPONSE) {
        //!! After the module successfully executes a command, it will return "OK\r\n"
        if (!strcmp(line, "OK\r\n") || !strcmp(line, "SEND OK\r\n"))
        {
            //!! The module returns "OK\r\n"
            AT_ApplyStateChange(AT_STATE_GOT_OK_RESPONSE);

            //!! Now, ready for next command
            AT_ApplyStateChange(AT_STATE_READY_FOR_COMMAND);

            //!! Free the memory. The memory contains the last command
            //!! After the cllback functions are called, DO NOT forget to free it
            AT_FreeMemory();
        }
        //!! After the mould is reset, the "ready\r\n" will be returned
        else if (!strcmp(line, "ready\r\n")) {
            //!! The module is detected
            AT_ApplyStateChange(AT_STATE_MODULE_CONNECTEED);

            //!! It ready for receiving some commands
            //!! The at.state must ne changed to AT_STATE_READY_FOR_COMMAND in this time
            AT_ApplyStateChange(AT_STATE_READY_FOR_COMMAND);
        }
    }
    return at.state;
}

//!! Must be called periodically, e.g.; every 100 mS
void AT_Loop(void *evt)
{
    static uint8_t prev_sec = 0;
    static uint16_t waitingSec = 0;
    os_time_t time = OS_TimeGet();

    if (at.state == AT_STATE_READY_FOR_COMMAND)
    {
        //!! Fetch a command in the command-queue
        if (AT_FetchCommand())
        {
            //!! Sent the command to ESP8266. The at.state will be changed in AT_ProcessLine()
            AT_ApplyStateChange(AT_STATE_WAIT_OK_RESPONSE);
            Uart2_AsyncWriteString(at.currentComd);

            //!! Debug
            Uart1_AsyncWriteString("\r\nAT Send >>> ");
            Uart1_AsyncWriteString(at.currentComd);
            Uart1_AsyncWriteString("\r\n");
  
            //!! Command is inserted in queue, speed up the AT_Loop timer to 100 mS
            //@@ timer_t *t = ((event_t *)evt)->sender;
            //@@ OS_TimerSetTicks(t, 100);
        }
        else {
            //!! No more command in the queue, slow down the AT_Loop timer to 1 second
            //@@ timer_t *t = ((event_t *)evt)->sender;
            //@@ OS_TimerSetTicks(t, 1000);
        }
    }

    //!!
    //!! 1 second periodically checking
    //!!
    if (time.ss != prev_sec) {
        //!! Save the current suconds
        prev_sec = time.ss;

        if (at.state == AT_STATE_WAIT_OK_RESPONSE) {
            if (++waitingSec >= 10)
            {
                //!! 10 seconds timeout! no token, "OK\r\n" is received
                waitingSec = 0;
                AT_ApplyStateChange(AT_STATE_WAIT_TIMEOUT);

                //!!
                //!! User should do something in the timeout callback function!!
                //!!

                //!! No "OK\r\n" response form the module
                //!! Change the at.state to AT_STATE_READY_FOR_COMMAND 
                //!! to allow the state machines can fetch a new command
                AT_ApplyStateChange(AT_STATE_READY_FOR_COMMAND);
            }
        }
        else {
            //!! reset the counter
            waitingSec = 0;
        }
    }
}
//!! EOF
