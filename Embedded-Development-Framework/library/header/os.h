/************************************************************
 * File:    os.h                                            *
 * Author:  Dr.Santi Nuratch                                *
 *          Embedded Computing and Control Laboratory       *
 * Update:  16 July 2017, 10:09 PM                          *
 ************************************************************/

#ifndef __OS_H__
#define __OS_H__

#include "config.h"
#include "mcu.h"
#include "data.h"
#include "str.h"
#include "event.h"
#include "uart.h"
#include "queue.h"
#include "led.h"
#include "os.h"
#include "stimer.h"
#include "worker.h"
#include "psw.h"
#include "adc.h"
#include "beep.h"
#include "looper.h"
#include "stdarg.h"

typedef void (*os_callback_t)(void *);

#define U1_EVT_RX_RECV 0  // u1 rx byte received
#define U2_EVT_RX_RECV 1  // u1 rx byte received
#define U1_EVT_TX_EMPTY 2 // u1 tx bffer empty
#define U2_EVT_TX_EMPTY 3 // u2 tx bffer empty

#define U1_EVT_LINE_RECV 4 // u1 rx line received
#define U2_EVT_LINE_RECV 5 // u2 rx line received

#define U2_EVT_ESP_RECV 6 // wifi (u2 rx) frame received

typedef struct
{
    uint8_t type; // UART event type
    uint8_t id;   // UART id, UART_ID_1 or UART_ID_2
    data_t data;  // UART data
} uart_event_t;

typedef struct
{
    os_callback_t u1_rx_callback; // A byte is received
    os_callback_t u1_tx_callback; // All bytes are transmitted

    os_callback_t u2_rx_callback; // A byte is received
    os_callback_t u2_tx_callback; // All bytes are transmitted

    os_callback_t system_tick_callback; // The system is ticked (1 mS)

    os_callback_t uart1_line_received; // UART1 line received callback
    os_callback_t uart2_line_received; // UART2 line received callback

    os_callback_t esp8266_frame_received; // +PID,.,..:<data>\r\n

} os_system_callback_t;

typedef struct
{
    uint16_t tick_count;
    uint16_t ticked_flag;
    uint32_t time_ms;
    uint16_t need_update;
} os_parameter_t;

typedef struct
{
    uint16_t us;
    uint16_t ms;
    uint16_t ss;
    uint16_t mm;
    uint16_t hh;
} os_time_t;

typedef struct
{
    uint8_t *data;
    uint8_t length;
} uart_line_t;

typedef struct
{
    uint8_t *data;
    uint16_t length;
} os_data_t;

void OS_Initialise(void);
void OS_SystemTickSetCallback(uint16_t ticks, os_callback_t callback);
void OS_Uart1SetRxCallback(os_callback_t callback);
void OS_Uart1SetLineReceivedCallback(os_callback_t callback);
void OS_Uart1SetTxCallback(os_callback_t callback);
void OS_Uart2SetRxCallback(os_callback_t callback);
void OS_Uart2SetLineReceivedCallback(os_callback_t callback);
void OS_Uart2SetTxCallback(os_callback_t callback);

void OS_WiFiSetFrameReceivedCallback(os_callback_t callback);

os_time_t OS_TimeGet(void);
void OS_TimeSet(uint8_t hh, uint8_t mm, uint8_t ss);
double OS_TimeHighResolution(void);
uint32_t OS_TimeMicroseconds(void);
uint32_t OS_TimeMilliseconds(void);
uint8_t OS_TickedCheck(void);
void OS_TimerCoreInit(void);
void OS_Sleep(uint16_t ticks);

void OS_Execute(void);

//
// Internally call function
//
void OS_TimeService(void);

#define OS_Init()                     \
    {                                 \
        MCU_Init();                   \
        __delay_ms(1000);             \
        UART_Init(UART_ID_1, 115200); \
        UART_Init(UART_ID_2, 115200); \
        LED_Init();                   \
        ADC_Init();                   \
        PSW_Init();                   \
        Beep_Init();                  \
        OS_LooperInit();              \
        OS_Initialise();              \
    }

#define OS_Start()          \
    {                       \
        while (1)           \
        {                   \
            OS_Execute();   \
            OS_EventLoop(); \
        }                   \
    }

//!!
//!! UserService
//!!

#endif
