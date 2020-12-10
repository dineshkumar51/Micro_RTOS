#include "osKernel.h"

#define NUM_OF_THREADS 3
#define STACKSIZE      100
#define BUS_FREQ       16000000

#define SYSPRI3        (*((volatile uint32_t *) 0xE000ED20))
#define INTCTRL        (*((volatile uint32_t *) 0xE000ED04))   

uint32_t MILLIS_PRESCALAR;
void osSchedulerLaunch(void);


typedef struct tcb{
	uint32_t *stackPt;
  struct tcb *next;	
}tcbType;

tcbType tcbs[NUM_OF_THREADS];

tcbType *currentPt;

uint32_t TCB_STACK[NUM_OF_THREADS][STACKSIZE];

void osKernelStackInit(int i)
{
	tcbs[i].stackPt = &TCB_STACK[i][STACKSIZE-16];
	TCB_STACK[i][STACKSIZE-1] = 0x01000000;
}

uint8_t osKernelAddThreads(void(*task0)(void), void(*task1)(void), void(*task2)(void))
{
	
	__disable_irq();
	tcbs[0].next = &tcbs[1];
	tcbs[1].next = &tcbs[2];
	tcbs[2].next = &tcbs[0];
	
	osKernelStackInit(0);
	TCB_STACK[0][STACKSIZE-2] = (uint32_t)(task0);
	
	osKernelStackInit(1);
	TCB_STACK[1][STACKSIZE-2] = (uint32_t)(task1);
	
	osKernelStackInit(2);
	TCB_STACK[2][STACKSIZE-2] = (uint32_t)(task2);
	currentPt = &tcbs[0];
	
	__enable_irq();
	
  return 1;
}

void osKernelInit(void)
{
	__disable_irq();
	
	MILLIS_PRESCALAR = BUS_FREQ/1000;
	
}

void osKernelLaunch(uint32_t quanta)
{
	SysTick->CTRL = 0;
	SysTick->VAL = 0;
	SYSPRI3 = (SYSPRI3&0x00FFFFFF) | 0xE0000000;
	SysTick->LOAD = (quanta*MILLIS_PRESCALAR)-1;
	SysTick->CTRL = 0x00000007;
	
	osSchedulerLaunch();
	
}

void osThreadYield(void)
{
	INTCTRL |= (1<<26);//trigger Systick
}
	
uint32_t periodic_tick;
#define PERIOD  100
void task3(void);

void choose_next(void)
{
	if(++ periodic_tick==PERIOD)
	{
		(*task3)();
	}
	currentPt = currentPt->next;
}




//schedular
//TCB
//STACK


