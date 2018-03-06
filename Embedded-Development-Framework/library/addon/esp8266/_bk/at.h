#ifndef __AT_COMMAND__
#define __AT_COMMAND__

#include "os.h"


#define AT_STATE_MODULE_DISCONNECTED    0       //!! [0] No module is detected
#define AT_STATE_MODULE_CONNECTEED      1       //!! [1] Module is detected/connected
#define AT_STATE_READY_FOR_COMMAND      2       //!! [2] Waiting for a next command
#define AT_STATE_WAIT_OK_RESPONSE       3       //!! [3] Waiting for response message
#define AT_STATE_GOT_OK_RESPONSE        4       //!! [4] Received response message
#define AT_STATE_WAIT_TIMEOUT           5       //!! [5] No response message is received


#define AT_QUEUE_LENGTH                 8       //!! AT command queue length
typedef struct
{
    uint8_t state;                              //!! state
    char *pComds[AT_QUEUE_LENGTH];              //!! Array of pointers pointing to commands
    char *currentComd;                          //!! Pointer to current command
    os_callback_t stateChangedCallback;         //!! State changed callback function         
    uint8_t put;                                //!! Queue put index
    uint8_t get;                                //!! Queue get index
    uint8_t cnt;                                //!! Number of commands in queue
} at_command_t;

//!! These two variables are desined in at.c
extern at_command_t at;                         //!! AT command structure
extern const char *AtStateMessage[];            //!! AT state essages for debuging

//!! Initial all parameters of the at and assign state changed callback function
//!! This function must be called in startup
void AT_CommandInit(os_callback_t stateChangedCallback);

//!! Put command into queue
//!! This function allocates memory from heap, 
//!! don't forget to free the memory
bool AT_PutCommand(char *command);

//!! Fetch/Get command from queue
//!! The command will be pointed by the at.currentComd
bool AT_FetchCommand(void);

//!! Free memory pointed by the at.currentComd
void AT_FreeMemory(void);

//!! Process all received lines
//!! This function must be called from the LineReceived callback of the ESP/UART2
uint16_t AT_ProcessLine(const char *line);

//!! AT main loop processing
//!! This function is a callback function of timer
//!! Don't forget to creat a timer and give this function as its callback
void AT_Loop(void *evt);

#endif // __AT_COMMAND__
//!! EOF
