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









void osThreadYield(void);

uint8_t osKernelAddThreads(void(*task0)(void),
						   void(*task1)(void),
						   void(*task2)(void));
						   
void choose_next(void); 

#endif


