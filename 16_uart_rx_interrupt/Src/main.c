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
	// Give clock access to GPIO port A
	RCC->AHB1ENR |= GPIOAEN;
	// Configure MODE register to be in output mode
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~(1U<<11);

	uart2_rxtx_interrupt_init();

	while (1)
	{

	}
}

static void uart_callback(void)
{
	key = USART2->DR;
	if (key == '1')
	{
		GPIOA->ODR |= LED_PIN;
	}
	else {
		GPIOA->ODR &= ~LED_PIN;
	}
}

void USART2_IRQHandler(void)
{
	/* check if RXNE is set */
	if (USART2->SR & SR_RXNE)
	{
		// Do something
		uart_callback();
	}
}
