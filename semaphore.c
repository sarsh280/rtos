#include "lab_executive.h"

#define MAX_SEMAPHORES 10

/*
Name: Enum SemaphoreState_e
Description: Semaphore State 
*/
typedef enum SemaphoreState_e
{
    SEMAPHORE_FREE,
    SEMAPHORE_ALLOCATED,
} SemaphoreState_t;

/*
Name: Semophore Structure
Description: State, count, taskID, Name
*/

typedef struct Semaphore_s
{
    SemaphoreState_t state; /* State of semaphore */
    int32_t count;          /* Semaphore Count */
    int32_t taskID;         /* Task ID of task */
    char *name;             /* Name of semaphore */
} Semaphores;

/*
Name: SemaphoreInit
Description: Semaphore Initialization
*/

void SemaphoreInit(void)
{
    uint32_t i;
    /* Initialize all semaphores to free */
    for (i = 0; i < MAX_SEMAPHORES; i++)
    {
        Semaphores[i].state = SEMAPHORE_FREE;
        Semaphores[i].count = 0;
        Semaphores[i].taskID = -1;
        Semaphores[i].name = NULL;
    }
}

/*
Name: SemaphoreNew
Description: Making new Semaphore
*/

uint32_t SemaphoreNew(uint32_t *semaphoreHandle, int32_t count, char *name)
{
    uint32_t i;
    if (semaphoreHandle == NULL)
    {
        printf("Error: NULL Semaphore handle passed "
               "to SemaphoreNew()\n");
        return 1;
    }
    for (i = 0; i < MAX_SEMAPHORES; i++)
    {
        if (Semaphores[i].state == SEMAPHORE_FREE)
        {
            Semaphores[i].state = SEMAPHORE_ALLOCATED;
            Semaphores[i].count = count;
            Semaphores[i].taskID = -1;
            Semaphores[i].name = name;
            *semaphoreHandle = i;
            return 0;
        }
    }
    printf("Error: Out of semaphores to allocate "
           "in SemaphoreNew()\n");
    return 1;
}

/*
Name: 
Description: 
*/

uint32_t SemaphorePost(uint32_t semaphoreHandle)
{
    if (semaphoreHandle >= MAX_SEMAPHORES)
    {
        printf("Error: semaphoreHandle is out "
               "of range in SemaphorePost()\n");
        return 1;
    }
    if (Semaphores[semaphoreHandle].state == SEMAPHORE_FREE)
    {
        printf("Error: trying to post to an "
               "unallocated "
               "semaphore in SemaphorePost()\n");
        return 1;
    }
    /* Post the semaphore */
    Semaphores[semaphoreHandle].count++;
    /* Can we unblock a task now */
    if ((Semaphores[semaphoreHandle].count > 0) && (Semaphores[semaphoreHandle].taskID != -1))
    {
        /* Yes, count is greater than 0,
         * and there is a task waiting */
        Semaphores[semaphoreHandle].count--;
        TaskReady(Semaphores[semaphoreHandle].taskID);
        Semaphores[semaphoreHandle].taskID = -1;
    }
    return 0;
}

/*
Name: 
Description: 
*/

uint32_t SemaphoreAcquire(uint32_t semaphoreHandle)
{
    if (semaphoreHandle >= MAX_SEMAPHORES)
    {
        printf("Error: semaphoreHandle is out of "
               "range in SemaphoreAcquire()\n");
        return 1;
    }

    if (Semaphores[semaphoreHandle].state == SEMAPHORE_FREE)
    {
        printf("Error: trying to acquire an "
               "unallocated "
               "semaphore in SemaphoreAcquire()\n");
        return 1;
    }

    if (Semaphores[semaphoreHandle].taskID != -1)
    {
        printf("Error: There is a task already waiting "
               "on the semaphore in "
               "SemaphoreAcquire()\n");
        return 1;
    }

    /* Check if we need to block */
    Semaphores[semaphoreHandle].count--;
    if (Semaphores[semaphoreHandle].count <= 0)
    {
        /* Yes, there are no counts left */
        Semaphores[semaphoreHandle].taskID =
            TaskCurrent();
        TaskPending(TaskCurrent());
        return 1;
    }
    return 0;
}


