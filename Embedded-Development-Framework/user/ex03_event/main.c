#include "os.h"

int main(void) {

    int tick_count = 0;

	OS_Init();      // Initialise the OS
    while(1)
    {
        if( OS_TickedCheck() )
        {
            LED0_Inv();     // Toggle LED0, output freq==500Hz

            tick_count++;   // Increase the counter by 1
            if(tick_count >= 500) { // Is it 500mS?
                tick_count = 0;     // Reset the counter
                LED_Inv(LED_ID_3);  // Invert the LED3
                                    // The outpur freq == 1Hz
            }
        }
    }

    OS_Start();      // Not required for this example
}
