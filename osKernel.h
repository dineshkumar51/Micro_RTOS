#ifndef _OS_KERNEL_H
#define _OS_KERNEL_H

#include<stdint.h>
#include "stm32f4xx.h"

void osKernelLaunch(uint32_t quanta);

void osKernelInit(void);

void task0(void);
void task1(void);
void task2(void);

#define PERIOD  100
void task3(void);

void osSemaphoreInit(int32_t *semaphore, int32_t value);
void osSignalset(int *semaphore);
void osSignalwait(int32_t *semaphore);



void osMailboxInit(void);
void osMailboxSend(uint32_t data);
uint32_t osMailboxReceive(void);

void osFifoInit(void);
int8_t osFifoPut(uint32_t data);
uint32_t osFifoGet(void);

void osThreadSleep(uint32_t sleep_ticks);









void osThreadYield(void);

uint8_t osKernelAddThreads(void(*task0)(void),uint32_t p0,
	                         void(*task1)(void),uint32_t p1,
		                       void(*task2)(void),uint32_t p2,
			                     void(*task3)(void),uint32_t p3);
						   
void choose_next(void); 

#endif


