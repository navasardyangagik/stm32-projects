#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"

#define GPIOAEN  		(1U<<0)
#define UART2EN			(1U<<17)

#define CR1_TE 			(1U<<3) // CONTROL register 1.
#define CR1_UE 			(1U<<13)
#define SR_TXE 			(1U<<7) // to check if uart tx is empty

#define SYS_FREQ 		16000000
#define APB1_CLK 		SYS_FREQ

#define UART_BAUDRATE 	115200


static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_BD(uint32_t PeriphClk, uint32_t BaudRate);

void uar2_tx_init(void);
void uart2_write(int ch);



int __io_putchar(int ch)
{
	uart2_write(ch);
	return ch;
}
int main(void)
{
	uar2_tx_init();

	while(1)
	{
		printf("Hello from STM32F4.........\n\r");
	}
}

void uar2_tx_init(void)
{
	// First configure uart gpio pin, it's an alternate function first of all.

	// enable clock access to gpioA
	RCC->AHB1ENR |= GPIOAEN;
	// Set PA2 mode to alternate function mode
	GPIOA->MODER |= (1U<<5);
	GPIOA->MODER &=~(1U<<4);

	// Set PA2 alternate function type to UART_TX. (AF07)
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &=~(1U<<11);


	// configure uart module module
	// Enable clock access to uart2.
	RCC->APB1ENR |= UART2EN;

	// configure the baudrate.
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	/* configure the transfer direction */
	USART2->CR1 = CR1_TE; // Deliberately use = so we clear everything other than enable. Sets defaults + parity.

	/* enable the uart module. Bit 13 is UE. Uart enable*/
	USART2->CR1 |= CR1_UE;
}

void uart2_write(int ch)
{
	// make sure the transmit data register is empty
	while(!(USART2->SR & SR_TXE)){} // When does this return true? i have no idea.

	// Write to transmit data register
	USART2->DR = (ch & 0xFF);
}


static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
	USARTx->BRR = compute_uart_BD(PeriphClk, BaudRate);
}

static uint16_t compute_uart_BD(uint32_t PeriphClk, uint32_t BaudRate)
{
	return ((PeriphClk + (BaudRate/2U))/BaudRate);
}
