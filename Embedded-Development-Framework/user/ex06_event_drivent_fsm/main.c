#include "os.h"

uint8_t counter = 0;
uint8_t state = 0;  // State #0, S0

// Pointer to timer object/structure
timer_t *timer;

void TurnMachineOFF(void *unused)
{
    LED0_Off();     // Turn OFF LED0 
    OS_TimerDelete(timer);
    counter = 0;    // Reset the counter to 0
    Beep(1000);
}

void FSM(void *param) {
    counter = counter+1;  

    if(counter >= 5) {
        state = 1;
        LED0_On();  // Turn ON LED0  
        timer = OS_TimerCreate("Timer", 5*1000, TIMER_MODE_ONESHORT, TurnMachineOFF);
        Beep(100);
    }
    else {
        state = 0;  // not required in this example
    }

    char buff[32];
    sprintf(buff, "\r\nSTATE:%d, Count: %d", state, counter);
    UART1_AsyncWriteString(buff);
}


int main(void) {
	OS_Init();          // Initialise the OS

    // Register the event of the PSW0
    OS_SwitchSetCallback(PSW_ID_0, FSM);

    OS_Start();         // Start the OS
}
