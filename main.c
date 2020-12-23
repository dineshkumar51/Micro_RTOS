#include "osKernel.h"
#include "stm32f4xx.h"

#define QUANTA  10

uint32_t BSP_Sensor_Read(void);
void BSP_ADC1_Init(void);

static uint32_t count0 = 0;
static uint32_t count1 = 0;
static uint32_t count2 = 0;
static uint32_t count3 = 0;


int32_t semaphore1;
int32_t semaphore2;


uint32_t sensor_val = 324;
void task0(void)
{
	while(1)
	{
		//osFifoPut(BSP_Sensor_Read());
		//osSignalwait(&semaphore1);
		count0++;
		//osSignalset(&semaphore2);
		//osThreadSleep(10);
		//osThreadYield();
	}
}
 
uint32_t data1;

void task1(void)
{
	while(1)
	{
		//data1 = osFifoGet();
		//osSignalwait(&semaphore2);
		count1++;
		osThreadSleep(2);
		//osSignalset(&semaphore1);
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
	while(1)
	{
		count3++;
	}
}








int main(void)
{
	
	//osSemaphoreInit(&semaphore1,1);
	//osSemaphoreInit(&semaphore2,0);
	BSP_ADC1_Init();
	osKernelInit();
	osFifoInit();
	osKernelAddThreads(&task0,2,&task1,1,&task2,5,&task3,1);
	osKernelLaunch(QUANTA);
}

void BSP_ADC1_Init(void){
  
	  /* set up pin PA1 for analog input */
    RCC->AHB1ENR |=  1;	            /* enable GPIOA clock */
    GPIOA->MODER |=  0xC;           /* PA1 analog */

      /* setup ADC1 */
    RCC->APB2ENR |= 0x00000100;     /* enable ADC1 clock */
    ADC1->CR2 = 0;                  /* SW trigger */
    ADC1->SQR3 = 1;                 /* conversion sequence starts at ch 1 */
    ADC1->SQR1 = 0;                 /* conversion sequence length 1 */
    ADC1->CR2 |= 1;                 /* enable ADC1 */
 
}

uint32_t BSP_Sensor_Read(void){
    
		ADC1->CR2 |= 0x40000000;        /* start a conversion */
    while(!(ADC1->SR & 2)) {}       /* wait for conv complete */
  
	  return ADC1->DR;

}
