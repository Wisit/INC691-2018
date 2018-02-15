#include "os.h"

// Callback function
void Worker1(void *param) {
    LED_Toggle(LED_ID_1);
}

// Callback function
void Worker2(void *param) {
    LED_Toggle(LED_ID_3);
}


int main(void) {
	OS_Init();          // Initialise the OS

    // Crdate timer and its callback
    OS_TimerCreate(
            "T1",       // Timer name
            1000,       // time
            TIMER_MODE_CONTINUEOUS, // mode 
            Worker1     // callback function
    );

    // Crdate timer and its callback
    OS_TimerCreate(
            "T2",       // Timer name
            50,         // time 50mS (20Hz)
            TIMER_MODE_CONTINUEOUS, // mode 
            Worker2     // callback function
    );
    OS_Start();         // Start the OS
}
