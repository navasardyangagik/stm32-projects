#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"

// Here, since we dont actually have a terminal on our stm to test the characters to display, we want to do it through flashing the LED for certain chars

#define GPIOAEN 	(1U<<0)
#define GPIOA_5		(1U<<5)

#define LED_PIN		GPIOA_5

char key;

int main(void)
{
	char message[31] = "Hello from Stm32 DMA transfer \n\r";

	// Give clock access to GPIO port A
	RCC->AHB1ENR |= GPIOAEN;
	// Configure MODE register to be in output mode
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~(1U<<11);

	uart2_tx_init();
	dma1_stream6_init((uint32_t)message, (uint32_t)&USART2->DR, 31);

	while (1)
	{

	}
}

static void dma_callback(void)
{
	GPIOA->ODR |= LED_PIN;
}

void DMA1_Stream6_IRQHandler(void)
{
	if (DMA1->HISR & HISR_TCIF6)
	{
		/* clear flag */
		DMA1->HIFCR |= HIFCR_CTCIF6;

		/* do something */
		dma_callback();
	}
}
