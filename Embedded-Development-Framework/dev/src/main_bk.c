#include "os.h"



void u1_callback(void *evt) {
	uart_event_t *e = (uart_event_t *)evt;
	uint8_t c = e->data.buffer[0];
	
	if(c == '\r') {
		UART1_AsyncWriteString("\r\nCR");
	}
	else if( c == '\n') {
		UART1_AsyncWriteString("\r\nLF\r\n");
	}
	
	else {
		UART1_AsyncPutByte('[');
		UART1_AsyncWriteBytes(&c, e->data.length);
		UART1_AsyncPutByte(']');
	}
    //uart_async_put_byte(UART_ID_1, *e->data.buffer);
    //UART1_AsyncWriteData(&e->data);
    //UART_AsyncWriteData(1, &e->data);
    //uart_async_write_data(UART_ID_1, &e->data);
	//UART1_AsyncWriteString("#");
}

void u1_line_recv(void *evt) {
    uart_event_t *e = (uart_event_t *)evt;
    UART1_AsyncPutByte('{');
    int i;
    for(i=0; i<e->data.length; i++) {
        uint8_t c = e->data.buffer[i];
        if(c == '\r') {
            UART1_AsyncWriteString("[CR]");
        }
        else if(c == '\n') {
            UART1_AsyncWriteString("[LF]");
        }
        else{
            UART1_AsyncPutByte(c);
        }
    }
    //UART_AsyncWriteData(1, &e->data);
    UART1_AsyncWriteString("}\r\n");
}


void t1(void *evt) {
    event_t *e = (event_t *)evt;
    timer_t *t = (timer_t *)e->sender;
    UART_AsyncWriteString(1, t->name);
    //UART_AsyncWriteData(1, &e->data);
    LED3_Inv();

}


char g_buffer[64];

void worker1(void *evt) {
    event_t *e = (event_t *)evt;
    worker_t *w = (worker_t *)e->sender;
    sprintf(g_buffer, "\r\nADC[0]: %3.3f volts", (double)ADC_Get(0)*3.3/1203);
    UART1_AsyncWriteString(g_buffer);
    //UART_AsyncWriteString(1, w->name);
    OS_WorkerSleep(w, 800);
}

void w2(void *evt) {
    event_t *e = (event_t *)evt;
    worker_t *w = (worker_t *)e->sender;
    sprintf(g_buffer, "\r\nADC[1]: %3.3f volts", (double)ADC_Get(1)*3.3/1203);
    UART1_AsyncWriteString(g_buffer);
    //UART_AsyncWriteString(1, w->name);
    OS_WorkerSleep(w, 1000);
}

void w3(void *evt) {
    event_t *e = (event_t *)evt;
    worker_t *w = (worker_t *)e->sender;
    sprintf(g_buffer, "\r\nADC[2]: %3.3f volts", (float)ADC_Get(2)*3.3/1203);
    UART1_AsyncWriteString(g_buffer);
    //UART_AsyncWriteString(1, w->name);
    OS_WorkerSleep(w, 1200);
}

void w4(void *evt) {
    event_t *e = (event_t *)evt;
    worker_t *w = (worker_t *)e->sender;
    sprintf(g_buffer, "\r\nADC[3]: %3.3f volts", (double)ADC_Get(3)*3.3/1203);
    UART1_AsyncWriteString(g_buffer);
    //UART_AsyncWriteString(1, w->name);
    OS_WorkerSleep(w, 1500);
}

void w5(void *evt) {
    event_t *e = (event_t *)evt;
    worker_t *w = (worker_t *)e->sender;
    //UART1_AsyncWriteString(w->name);
    //OS_WorkerSleep(w, 1800);
    int i;
    for(i=3; i>=0; i--) {
        double adc = (double)ADC_Get(i)*3.3/1023;
        sprintf(g_buffer, "%sADC[%d]: %3.3f volts", (i==3)?"\r\n":", ", i, adc);
        UART1_AsyncWriteString(g_buffer);
    }   
    sprintf(g_buffer, " - @ %ld", OS_TimeMicroseconds());
    UART1_AsyncWriteString(g_buffer);
    OS_WorkerSleep(w, 100);
}


void psw_callback(void *evt) {
    event_t  *e = (event_t *)evt;
    switch_t *s = (switch_t *)e->sender;
    
    char buff[64];
    sprintf(buff, "\r\nPSWId: %d, status: %d, count: %d - @ %lu uS", s->id, s->status, s->counter, OS_TimeMicroseconds());
    UART1_AsyncWriteString(buff);
    
    if(s->status != PSW_STATUS_HOLD_FAST) {
        Beep(50);
    }
}

int main(void)  {

    OS_Init();
    
    
    UART1_AsyncWriteString("\r\nSystem Ready!\r\n");
    LED_PwmSet(LED_ID_0, 500, 0,  1);
    LED_PwmSet(LED_ID_1, 500, 50, 1);
    LED_PwmSet(LED_ID_2, 500, 100, 1);
    LED_PwmSet(LED_ID_3, 500, 150, 1);
    
    OS_SwitchSetCallback(PSW_ID_0, psw_callback);
    OS_SwitchSetCallback(PSW_ID_1, psw_callback);
    OS_SwitchSetCallback(PSW_ID_2, psw_callback);
    OS_SwitchSetCallback(PSW_ID_3, psw_callback);
	
	Beep_PowerSet(0.9);
    Beep(100);
    //Beep_PowerSet(0.2);
    
    //long x= 65537;
    //char buff[32];
    //sprintf(buff, "x=%ld %c %s %d", x, 'X', "Hello", 32);
    //UART1_AsyncWriteString(buff);
    //uart1_printf("x=%ld %c %s %d", (long)65537, 'X', "Hello", 32);
    
    
    //uart1_printf("dsc\r\n", 12, "Hello", 'X');
    //OS_Uart1SetRxCallback(u1_callback);
    // OS_Uart2SetRxCallback(u1_callback);
    
    //OS_Uart1SetLineReceivedCallback(u1_line_recv);
    // OS_Uart2SetLineReceivedCallback(u1_line_recv);
    // OS_TimerCreate("Timer#1", 1000, 1, t1);
    // OS_TimerCreate("Timer#2", 1000, 1, t1);
    // OS_TimerCreate("Timer#3", 1000, 1, t1);
    // OS_TimerCreate("Timer#4", 1000, 1, t1);
    // OS_TimerCreate("Timer#5", 1000, 1, t1);
    
    
    /*
     * time
     */
    
    //OS_WorkerCreate("Worker#1", worker1);
    //OS_WorkerCreate("Worker#2", w2);
    //OS_WorkerCreate("Worker#3", w3);
    //OS_WorkerCreate("Worker#4", w4);
    //OS_WorkerCreate("Worker#5", w5);


    OS_Start();

}



