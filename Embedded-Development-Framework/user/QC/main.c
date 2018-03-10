#include "os.h"

//!! UART1 Line Received Callback
void Uart1LineCallback(void *evt)
{
    //!! Pointer to void to pointer to uart_event_t
    uart_event_t *uart_event = (uart_event_t *)evt;

    //!! Data structure
    data_t uart_data = uart_event->data;

    //!! Line data
    const char *line_data = (const char *)uart_data.buffer;

    //!! Print received data
    char buff[32];
    sprintf(buff, "Received %d bytes: %s", uart_data.length, line_data);
    UART1_AsyncWriteString(buff);
}

void Uart2LineCallback(void *evt)
{
    //!! Pointer to void to pointer to uart_event_t
    uart_event_t *uart_event = (uart_event_t *)evt;

    //!! Data structure
    data_t uart_data = uart_event->data;

    //!! Line data
    const char *line_data = (const char *)uart_data.buffer;

    //!! Print received data
    Uart1_AsyncWriteBytes(line_data, uart_data.length);
}

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

}


int main(void) {
    OS_Init();

    // LED_ModeSet(LED_ID_0, LED_MODE_PWM);
    // LED_ModeSet(LED_ID_1, LED_MODE_PWM);
    // LED_ModeSet(LED_ID_2, LED_MODE_PWM);
    // LED_ModeSet(LED_ID_3, LED_MODE_PWM);

    // LED_PwmSet(LED_ID_0, 500,  0,   10);
    // LED_PwmSet(LED_ID_1, 500,  50,  10)
    // LED_PwmSet(LED_ID_2, 500,  100, 10);
    // LED_PwmSet(LED_ID_3, 500,  200, 10)


    Uart1_AsyncWriteString("OK\r\n");
    Uart2_AsyncWriteString("AT+GMR\r\n");

    Beep(100);


    int id;
    for(id=0; id<4; id++) {
        LED_ModeSet(id, LED_MODE_PWM);
        LED_PwmSet(id, 500,  id*50,   10);
        OS_SwitchSetCallback(id, SwitchCallback);
    }
    
    //!! Register UART1 Line Received Callback
    OS_Uart1SetLineReceivedCallback(Uart1LineCallback);
    OS_Uart2SetLineReceivedCallback(Uart2LineCallback);

    OS_Start();
}
