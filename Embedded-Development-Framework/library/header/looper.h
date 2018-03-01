#ifndef __LOOPER_H__
#define __LOOPER_H__

#include "os.h"

typedef void (*looper_callback_t)(void *);

#define LOOPER_MAX 5
typedef struct
{
    looper_callback_t callback;
    uint16_t sleepTicks;
    char name[16];
} looper_t;

void OS_LooperInit(void);
looper_t *OS_LooperCreate(char *name, looper_callback_t callback);
void OS_LooperSetCallback(looper_t *looper, looper_callback_t callback);
void OS_LooperKill(looper_t *looper);
void OS_LooperSleep(looper_t *looper, uint16_t sleepTicks);
void OS_LooperWeakup(looper_t *looper);

void OS_LooperService(void);
#endif
