#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "tim.h"


#define GPIOAEN 		(1U<<0)
#define PIN5			(1U<<5)

#define LED 			PIN5

uint32_t sensor_value;

int main(void)
{

	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~(1U<<11);

	uart2_tx_init();
	tim2_1hz_init();

	while (1)
	{
		/* wait for UIF */
		while (!(TIM2->SR & SR_UIF)){}
		/* clear UIF*/
		TIM2->SR &=~(SR_UIF);

		// We need to wait for a particular flag that indicates the timeout has occured. (The count has been reached.) This is UIF
		printf("A second passed !! \n \r");
		GPIOA->ODR ^= LED;
	}
}
