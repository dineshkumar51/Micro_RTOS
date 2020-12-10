#include "osKernel.h"


#define NUM_OF_THREADS 4
#define STACKSIZE      100
#define BUS_FREQ       16000000

#define SYSPRI3        (*((volatile uint32_t *) 0xE000ED20))
#define INTCTRL        (*((volatile uint32_t *) 0xE000ED04))   
	
#define FIFO_SIZE      15

uint32_t MILLIS_PRESCALAR;
void osSchedulerLaunch(void);


typedef struct tcb{
	uint32_t *stackPt;
  struct tcb *next;	
	uint32_t sleepTime;
	uint32_t blocked;
	uint32_t priority;
}tcbType;

tcbType tcbs[NUM_OF_THREADS];

tcbType *currentPt;

uint32_t TCB_STACK[NUM_OF_THREADS][STACKSIZE];

void osKernelStackInit(int i)
{
	tcbs[i].stackPt = &TCB_STACK[i][STACKSIZE-16];
	TCB_STACK[i][STACKSIZE-1] = 0x01000000;
}

uint8_t osKernelAddThreads(void(*task0)(void),uint32_t p0,
	                         void(*task1)(void),uint32_t p1,
		                       void(*task2)(void),uint32_t p2,
			                     void(*task3)(void),uint32_t p3)
														 
														 
{
	__disable_irq();
	
	tcbs[0].next = &tcbs[1];
	tcbs[1].next = &tcbs[2];
	tcbs[2].next = &tcbs[3];
	tcbs[3].next = &tcbs[0];
	
	osKernelStackInit(0);
	TCB_STACK[0][STACKSIZE-2] = (uint32_t)(task0);
	
	osKernelStackInit(1);
	TCB_STACK[1][STACKSIZE-2] = (uint32_t)(task1);
	
	osKernelStackInit(2);
	TCB_STACK[2][STACKSIZE-2] = (uint32_t)(task2);
	
	osKernelStackInit(3);
	TCB_STACK[3][STACKSIZE-2] = (uint32_t)(task3);
	currentPt = &tcbs[0];
	
	for(int i=0; i<NUM_OF_THREADS; i++)
	{
		tcbs[i].sleepTime = 0;
		tcbs[i].blocked = 0;
	}
	
	tcbs[0].priority = p0;
	tcbs[1].priority = p1;
	tcbs[2].priority = p2;
	tcbs[3].priority = p3;
	
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


void osSemaphoreInit(int32_t *semaphore, int32_t value)
{
	*semaphore = value;
}


void osSignalset(int *semaphore)
{
	__disable_irq();
	*semaphore += 1;
	__enable_irq();
}


void osSignalwait(int32_t *semaphore)
{
	__disable_irq();
	
	while(*semaphore <= 0)
	{
		__disable_irq();
		osThreadYield();
		__enable_irq();
		
	}
	*semaphore -=1;
	__enable_irq();
}
	
uint32_t periodic_tick;
#define PERIOD  100
void periodic_task(void);




void choose_next(void)
{
	if(++ periodic_tick==PERIOD)
	{
		(*periodic_task)();
		periodic_tick = 0;
	}
	
	if(currentPt->sleepTime > 0)
	{
		currentPt =currentPt->next;
	}
	
	tcbType *__currentPt = currentPt;
	tcbType *nextThreadToRun = currentPt;
	uint8_t highestPriority = 255;
	
	do{
			__currentPt = __currentPt->next;
			if((__currentPt->priority < highestPriority) && (__currentPt->blocked == 0) && (__currentPt->sleepTime == 0))
			{
				nextThreadToRun = __currentPt;
				highestPriority = __currentPt->priority;
			}
		
			
		}while(__currentPt != currentPt);

		currentPt = nextThreadToRun;
}




static uint8_t MB_hasdata;
static uint32_t MB_data;
static int32_t  MB_sem;

void osMailboxInit(void)
{
	MB_hasdata = 0;
	MB_data = 0;
	osSemaphoreInit(&MB_sem,0);
}

void osMailboxSend(uint32_t data)
{
	__disable_irq();
	if(MB_hasdata)
	{
		__enable_irq();
		return;
	}
	
	MB_data = data;
	MB_hasdata = 1;
	__enable_irq();
	
	osSignalset(&MB_sem);
}


uint32_t osMailboxReceive(void)
{
	osSignalwait(&MB_sem);
	uint32_t data;
	__disable_irq();
	data = MB_data;
	MB_hasdata = 0;
	__enable_irq();
	return data;
}


uint32_t PutI;
uint32_t GetI;
uint32_t os_fifo[FIFO_SIZE];
int32_t current_fifo_size;
uint32_t lost_data;


void osFifoInit(void)
{
	PutI = 0;
	GetI = 0;
	osSemaphoreInit(&current_fifo_size,0);
	lost_data = 0;
}

int8_t osFifoPut(uint32_t data)
{
	if(current_fifo_size == FIFO_SIZE)
	{
		lost_data++;
		return -1;
	}
	os_fifo[PutI] = data;
	PutI = (PutI +1)% FIFO_SIZE;
	osSignalset(&current_fifo_size);
	return 1;
}

uint32_t osFifoGet(void)
{
	uint32_t data;
	osSignalwait(&current_fifo_size);
	__disable_irq();
	data = os_fifo[GetI];
	GetI  = (GetI +1)%FIFO_SIZE;
	__enable_irq();
	 return data;
}



void periodic_task(void)
{
	int i=0;
	while(i<NUM_OF_THREADS)
	{
		if(tcbs[i].sleepTime > 0)
		{
				tcbs[i].sleepTime--;
		}
		i++;
	
	}
	
}


void osThreadSleep(uint32_t sleep_ticks)
{
	__disable_irq();
	currentPt->sleepTime = sleep_ticks;
	__enable_irq();
	
	osThreadYield();
}


void SysTick_Handler(void)
{
	//Trigger PendSV
	INTCTRL = 0x10000000;
	
}




//schedular
//TCB
//STACK


