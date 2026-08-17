#include <stdint.h>
#include "stm32f4xx.h"

#define GPIOAEN  		(1U<<0)
#define UART2EN 		(1U<<17)

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);

/* We wish to use USART2. This is connected to the APB1 bus.
 * We first have to start the APB1 bus clock
 * We want transmission. TX.
 *
 *
 * */

int main(void)
{
	// Enable clock for GPIOA and USART2
	RCC->AHB1ENR |= GPIOAEN;
	RCC->APB1ENR |= UART2EN;


	// By checking data sheet grid. we see the pin defintions. For USART2_TX its specifically PA2 as an alternate function.
	// We need to go to the mode register and configure pin 2 to be a alternate function pin

	GPIOA->MODER |= (1U<<5);
	GPIOA->MODER &=~(1U<<4);

	// Then we further have to specify which type of alternate function it is. in our case USART TX. We operate in the low register because we're in the lower Pin region.
	// This is because the alternate function configurations are 4 bits wide, so there's only room for 8 pins in a register when in reality there's 16.
	// We know the bits we need to set because the alternate function mappings claimed USART 2 TX corresponds to AF7
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &=~(1U<<11);

	// Configure the USART2 configurations and enable it eventually in end
	USART2->BRR = compute_uart_bd(16000000, 115200);
	// We have to distinctly configure the transfer direction. If we enable the 3rd bit, TE. Transmitter enables.
	// By using simply = and not | we set the rest to zero giving us a default config parity etc.
	USART2->CR1 = (1U<<3);
	USART2->CR1 |= (1U<<13); // UE bit. bit 13 as 1 enables usart.
	while (1)
	{
		while (!(USART2->SR&(1U<<7))) {}

		USART2->DR = 'h' & 0xFF;
	}

}
// we actually need to do things like configuring the BAUD rate.
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate)
	{
		return ((PeriphClk + (BaudRate/2U)) / BaudRate);
	}
