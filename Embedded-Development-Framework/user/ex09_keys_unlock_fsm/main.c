#include "os.h"


#define S_Wait_A        0
#define S_Wait_B        1
#define S_Wait_C        2
#define S_Wait_D        3

int State = S_Wait_A;

// ABDC
void FSM_Unlocker(int key) {

    //!! Waiting for A (SW0)
    if( State == S_Wait_A ) {
        if(key == 0) { // 0 := Sw-A
            State = S_Wait_B;
        }
    }
    //!! Waiting for B (SW1)
    else if( State == S_Wait_B) {
        if(key == 1) { // 1 := Sw-B
            State = S_Wait_D;
        }
    }
    //!! Waiting for C (SW3)
    //!! This is ending state, Beep and back to S_Wait_A
    else if( State == S_Wait_C) {
        if(key == 2) { // 2 := Sw-C
            Beep(100);
            State = S_Wait_A;
        }       
    }
    //!! Waiting for D (SW3)
    else if( State == S_Wait_D) {
        if(key == 3) { // 3 := Sw-D
            State = S_Wait_C;
        }    
    }
}


//!! This callback function is shared for all switches (PBS<3:0>)
void SwitchCallback(void *param) {
    event_t *evt = (event_t *)param;
    switch_t *psw = evt->sender;
    FSM_Unlocker(psw->id);
}

int main(void) {
	OS_Init();

    OS_SwitchSetCallback(0, SwitchCallback);
    OS_SwitchSetCallback(1, SwitchCallback);
    OS_SwitchSetCallback(2, SwitchCallback);
    OS_SwitchSetCallback(3, SwitchCallback);

    OS_Start();
}
