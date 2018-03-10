#include "os.h"



int main(void) {
    
    //!!Variables and hardware initialization
    //!!...
	
    OS_Init();	// Initialise the OS

    Beep(100);
    Uart1_AsyncWriteString("\r\nReady!!\r\n");
	
    OS_Start();	// Start the OS
}
