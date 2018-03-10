#include "os.h"

//!! This callback function is shared for all switches (PBS<3:0>)
void SwitchCallback(void *param) {
    //!! cast the event-data (param) to the event_t
    event_t *evt = (event_t *)param;

    //!! point to the switch object, the sender
    switch_t *psw = evt->sender;
    
    //!! Print information to the console
    char buff[32];

    //!! [1] using psw->id
    sprintf(buff, "\r\nPSW%d is pressed", psw->id);
    //!! [2] using evt->data.buffer[0]
    //sprintf(buff, "\r\nPSW%d is pressed", evt->data.buffer[0]);
    UART1_AsyncWriteString(buff);

    //!! Toggle the LED pointed by the Id of the switch
    LED_Toggle(psw->id);
}

int main(void) {
	OS_Init();          // Initialise the OS

    Uart1_AsyncWriteString("Press PSW and check the result on the terminal\r\n");
    Beep(100);

    //!! Register the event of the PSW0
    //OS_SwitchSetCallback(PSW_ID_0, SwitchCallback);

    //!! Register the event of the PSW1
    //OS_SwitchSetCallback(PSW_ID_1, SwitchCallback);

    //!! Register the event of the PSW2
    //OS_SwitchSetCallback(PSW_ID_2, SwitchCallback);

    //!! Register the event of the PSW3
    //OS_SwitchSetCallback(PSW_ID_3, SwitchCallback);


    //!! The events can be registered in the for loop like this
    int id;
    for(id=0; id<4; id++) {
        OS_SwitchSetCallback(id, SwitchCallback);
    }

    

    OS_Start();         // Start the OS
}
