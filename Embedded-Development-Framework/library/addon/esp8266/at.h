#ifndef __AT_COMMAND__
#define __AT_COMMAND__

#include "os.h"

#define AT_STATE_WAIT_COMMAND 0     //!! [0] Waiting for a next command
#define AT_STATE_WAIT_RESPONSE 1    //!! [1] Waiting for response message
#define AT_STATE_GOT_RESPONSE 2     //!! [2] Received response message
#define AT_STATE_WAIT_TIMEOUT 3     //!! [3] No response message is received



#define AT_QUEUE_LENGTH 8           //!! AT command queue length
typedef struct
{
    uint8_t state;                  //!! state
    timer_t *timer;
    char *pComds[AT_QUEUE_LENGTH];  //!! Array of pointers pointing to commands
    char *currentComd;              //!! Pointer to current command
    os_callback_t timeoutCallback;
    uint16_t timeoutTicks;
    uint8_t put;                    //!! Queue put index
    uint8_t get;                    //!! Queue get index
    uint8_t cnt;                    //!! Number of commands in queue
} at_command_t;

//!! These two variables are desined in at.c
extern at_command_t atComd;         //!! AT command structure
extern const char *AtStateMsg[];    //!! AT state essages for debuging

//!! Initial all parameters of the atComd and assign timeout callback function
//!! This function must me called in startup
void AT_CommandInit(os_callback_t callback);

//!! Put command into queue
//!! This function allocates memory from heap, 
//!! don't forget to free the memory 
void AT_PutCommand(char *command);

//!! Fetch/Get command from queue
//!! The command will be pointed by the atComd.currentComd
bool AT_FetchCommand(void);

//!! Free memory pointed by the atComd.currentComd
void AT_FreeMemory(void);

//!! Process all received lines
//!! This function must be called from the LineReceived callback of the ESP/UART2
int AT_ProcessLine(const char *line);

void AT_SetTimeoutCallback(os_callback_t callback);

//!! AT main loop processing
//!! This function is a callback function of timer
//!! Don't forget to creat a timer and give this function as its callback
void AT_Loop(void *evt);

#endif // __AT_COMMAND__
