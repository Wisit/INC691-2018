#include "os.h"

int main(void) {

    uint16_t ticks = 0;

    OS_Init();

    LED_Set(LED_ID_3);                  // Set LED3
	
    while(1) {
        
        if( OS_TickedCheck() ) {        // OS tick is activated/ticked every 1 mS

            ticks++;                    // Increase ticks by 1

            if (ticks > 500) {
                ticks = 0;
                LED_Inv(LED_ID_0);      // Toggle LED0
                LED_Toggle(LED_ID_3);   // Toggle LED3
            }
            else if (ticks % 250 == 0)  {
                LED_Inv(LED_ID_2);      // Toggle LED2
            }
            else if (ticks == 50) {
                LED_Set(LED_ID_1);      // Set LED1
            }
            else if (ticks == 90) {
                LED_Clr(LED_ID_1);      // Clear LED1
            }
        }
    }

    OS_Start();
}
