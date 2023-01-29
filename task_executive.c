
#include "task_executive.h"
#define MAX_TASKS 5
static int32_t TaskNext(void);

typedef enum {
TASK_READY = 0,
TASK_PENDING,
TASK_RUNNING,
TASK_INACTIVE,
TASK_INTERRUPTED,
TASK_STATE_MAX
} TaskState_e;

typedef struct Task_s
{
    void (*f)(void *data);
    void *data;
    TaskState_e state;
} Task_t;

Task_t tasks[MAX_TASKS];
int32_t currentTask = 0;

void TaskInit(void)
{
    currentTask = -1;
}

int32_t TaskAdd(void (*f)(void *data), void *data)
{  
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (tasks[i].f == NULL)
        {
            tasks[i].f = f;
            tasks[i].data = data;
            tasks[i].state = TASK_READY;
            TaskReady(i);
            return i;
        }
    }

    return -1;
}

int32_t TaskKill(int32_t id)
{
    tasks[id].f = NULL;
    tasks[id].data = NULL;
    tasks[id].state = TASK_INACTIVE;
    printf("Task%ld INACTIVE \n\r",id+1);
    return 0;
}

int32_t TaskCurrent(void)
{   
    return currentTask;
}


int32_t TaskSwitcher(void)
{
    currentTask = TaskNext();
    if (currentTask < 0)
    {
        return -1;
    }
    tasks[currentTask].f(tasks[currentTask].data);
    return 0;
}

static int32_t TaskNext(void)
{
    int32_t i;
    uint32_t count = 0;
    i = currentTask;
    do
    {
        i = (i + 1) % MAX_TASKS;
        count++;
    } while ((tasks[i].f == NULL) && (count <= MAX_TASKS));
    return (count <= MAX_TASKS) ? i : -1;
}

int32_t TaskPending(int32_t id){
    id++;
    printf("Task %ld Pending\n\r",id);
    return 0;
}

int32_t TaskReady(int32_t id){
    id++;
    printf("Tasks %ld Ready\n\r",id );
    return 0;
}
