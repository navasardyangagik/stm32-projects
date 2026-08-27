#include <stdint.h>
#include "stm32f4xx.h"
#include "exti.h"
#include "uart.h"


#define SR_CC1IF 	(1U<<1)
#define GPIOAEN 		(1U<<0)
#define PIN5			(1U<<5)
#define LED 			PIN5

void EXTI15_10_IRQHandler(void);

int main(void)
{
	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~(1U<<11);

	pc13_exti_init();
	uart2_tx_init();
	while (1)
	{

	}
}

static void exti_callback(void)
{
	printf("Hi Bro!... \n \r");
	GPIOA->ODR ^= LED;
}


void EXTI15_10_IRQHandler(void)
{
	/* we have to find a way to distinguish which interrupt within this range has been triggered. Since all EXTI from 10-15 are handled here */
	if ((EXTI->PR & LINE13)!=0)
	{
		/* clear the PR flag */
		EXTI->PR |= (LINE13);
		/* do something */
		exti_callback();
	}
}
