
#include"stdio.h"
#ifndef __TASKS_H__
#define __TASKS_H__
void TaskInit(void);
int32_t TaskAdd(void (*f)(void *data), void *data);
int32_t TaskKill(int32_t id);
int32_t TaskCurrent(void);
int32_t TaskSwitcher(void);

int32_t TaskPending(int32_t id);
int32_t TaskReady(int32_t id);

#endif
