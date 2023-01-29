/*
 * Name         :       swtimer.c
 *
 * Description  :       Implement Software Timers
 *
 * Date         :       November 27, 2022
 *
 * Author(s)    :       Arshdeep Singh
 * 						Rajvinder Kaur
 */

#include "common.h"
#include "sw_timer.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Defines */
#define NUM_SWTIMERS 10
#define TIMER_FREE 0
#define TIMER_ALLOCATED (1 << 0)
#define TIMER_COUNTING (1 << 1)

/* End Defines */



/* Typedefs */
typedef struct Timer_s {
    uint32_t flags;
    uint32_t count;
    void *data;
    void (*callback)(void *);
    char *name;
} Timer_t;
/* End Typedefs */

/* Globals */
static Timer_t timers[NUM_SWTIMERS];
static volatile uint32_t accum_ticks = 0;
/* End Globals */

/* 
* Function to initialize timer struct 
*/
void SWTimerInit(void)
{
    // Set entire array to 0
        /* code */
        memset(timers, 0, NUM_SWTIMERS * sizeof(Timer_t));
    
}

/* 
* Function to allocate a timer to a handle 
*/
uint32_t SWTimerAllocate(uint32_t *handle, void (*callback)(void *), void *data, char *name)
{
    for (int i = 0; i < NUM_SWTIMERS; i++) {
        if (timers[i].flags == TIMER_FREE) {
            // Found free slot
            timers[i].flags = TIMER_ALLOCATED;
            timers[i].callback = callback;
            timers[i].data = data;
            timers[i].name = name;
            timers[i].count = 0;
     
            *handle = i;

            return 0;    
        }
    }

    return 1;    
}

/* 
* Function to free an allocated timer 
*/
uint32_t SWTimerFree(uint32_t handle)
{
    // if handle is valid
    if (handle >= NUM_SWTIMERS) {
        return 1;
    }

    if (timers[handle].flags == TIMER_FREE) {
        return 1;
    }

    memset(&timers[handle], 0, sizeof(Timer_t));

    return 0;
}

/* 
* Function to set count of an allocated timer
*/
uint32_t SWTimerSet(uint32_t handle, uint32_t count)
{
    if (count == 0){
        printf("timer no longer counting \n\r");
        return 1;
    }
    else if (timers->flags == TIMER_FREE){
        printf("timer no longer counting \n\r");
        return 1;
    }
    else if (handle > NUM_SWTIMERS){
        printf("timer no longer counting \n\r");
        return 1;
    }
    timers[handle].count = count;
    timers[handle].flags = TIMER_COUNTING;

    return 0;
}

/* 
* Function to set data to be used when a timer counts to 0 
*/
uint32_t SWTimerSetData(uint32_t handle, void *data)
{
    // if handle is valid
    if (handle > NUM_SWTIMERS) {
        return 1;
    }

    if (timers[handle].flags == TIMER_FREE) {
        return 1;
    }

    timers[handle].data = data;

    return 0;
}

/* 
* Function to decrement a timer by elapsed number of ticks 
*/
void SWTimerTick(void)
{
    uint32_t ticks = 0;

    // Disable interrupts briefly and check current accumulated ticks
    __disable_irq();
    ticks = accum_ticks;
    accum_ticks = 0;
    __enable_irq();

    for (int i = 0; i < NUM_SWTIMERS; i++) {
        Timer_t *timer = &timers[i];
        if (timer->flags == TIMER_COUNTING) {
            if (timer->count > ticks) {
                // Won't underflow
                // Timer is not done yet
                timer->count -= ticks;          
            } else {
                // Timer is done
                timer->count = 0;

                // Call routine
                timer->callback(timer->data);

            }
        }
    }
}

/* 
* Callback function to accumulate systicks 
*/

void SWTimerTickHandler(void)
{
    accum_ticks++;
}

/* 
* Add Timer
*/
ParserReturnVal_t CmdDumpTimers(int mode)
{
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;

    for (uint32_t i = 0; i < NUM_SWTIMERS; i++) {
        printf("%3u: %6u 0x%08x %9s %8s %s\n",
               (unsigned int) i,
               (unsigned int) timers[i].count,
               (unsigned int) timers[i].data,
               timers[i].flags & TIMER_ALLOCATED ? "ALLOCATED" : "",
               timers[i].flags & TIMER_COUNTING ? "COUNTING" : "",
               timers[i].name != NULL ? timers[i].name : ""
               );
    }

    return CmdReturnOk;
}
ADD_CMD("timers", CmdDumpTimers, "Timers");

int data =5;

void Task0(void *data)
{
      
    printf("Task running \n\r");

  return;
}

void Task1(void *data)
{
    printf("Task running\n");

    return;
}

/*
Init peripherals
*/
ParserReturnVal_t initP(int mode)
{
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;
    /* Code */
    
    void SystemClock_Config(void);
    static void MX_GPIO_Init(void);
    static void MX_USART2_UART_Init(void);

    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();

    return CmdReturnOk;
}
ADD_CMD("initp", initP, " init Peripherals");

/*
* Init Timers
*/

ParserReturnVal_t initTimers(int mode)
{
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;
    /* Code */
    SWTimerInit();

    for (uint32_t i = 0; i < NUM_SWTIMERS; i++)
    {
        /* code */
        SWTimerAllocate(&i,Task0,SWTimerTickHandler,"Timer");
    }
    SWTimerSet(0,2);
    SWTimerSetData(0,Task0); 
    
    SWTimerSet(1,5);
    SWTimerSetData(1,Task1);

    return CmdReturnOk;
}
ADD_CMD("initTimers", initTimers, " Initialize Timers");

ParserReturnVal_t tickTim(int mode)
{
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;
    /* Code */
    SWTimerTickHandler();
    SWTimerTickHandler();
    SWTimerTickHandler();

    SWTimerTick();

    return CmdReturnOk;
}
ADD_CMD("ticktim", tickTim, " Timer ticks");

ParserReturnVal_t freeTim(int mode)
{
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;
    /* Code */

    for (int i = 0; i < NUM_SWTIMERS; i++)
    {
        /* code */
        SWTimerFree(i);
    }
    

    return CmdReturnOk;
}
ADD_CMD("freetim", freeTim, " free Timers");

ParserReturnVal_t sys(int mode)
{
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;
    /* Code */
    

    return CmdReturnOk;
  while (1)
  {
    /* USER CODE END WHILE */
		//if(HAL_GetTick() - delay_temp > delay_count){
			//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
			//delay_temp = HAL_GetTick();
		//}
		if(!delay_ms){
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			delay_ms = 500;
		}
		if(flag_sys){
			flag_sys = 0;
			if(delay_ms>0){
				delay_ms--;
			}
		}
}
ADD_CMD("sys", sys, " systick Timers");

