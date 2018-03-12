#include "os.h"


void timer1(void *param) {
    (void)param;
    LED_Inv(3);
}

void u1_rx(void *evt)
{
    uart_event_t *e = (uart_event_t *)evt;
    uint8_t c = e->data.buffer[0];

    if (c == '\r')
    {
        UART1_AsyncWriteString("\r\nCR");
    }
    else if (c == '\n')
    {
        UART1_AsyncWriteString("\r\nLF\r\n");
    }
    else
    {
        UART1_AsyncPutByte('[');
        UART1_AsyncWriteBytes(&c, e->data.length);
        UART1_AsyncPutByte(']');
    }
}

int main(void) {
    OS_Init();

    OS_Uart1SetRxCallback(u1_rx);
    OS_TimerCreate("t1", 1000, TIMER_MODE_CONTINUEOUS, timer1);

    OS_Start();
}