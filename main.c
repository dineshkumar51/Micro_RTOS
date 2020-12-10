#include "osKernel.h"

#define QUANTA  10

static uint32_t count0 = 0;
static uint32_t count1 = 0;
static uint32_t count2 = 0;

void task0(void)
{
	while(1)
	{
		count0++;
		//osThreadYield();
	}
}

void task1(void)
{
	while(1)
	{
		count1++;
	}
}


void task2(void)
{
	while(1)
	{
		count2++;
	}
}



void task3(void)
{
 uint32_t i = 1;
 i=i+1;
}




int main(void)
{
	osKernelInit();
	osKernelAddThreads(&task0,&task1,&task2);
	osKernelLaunch(QUANTA);
}


