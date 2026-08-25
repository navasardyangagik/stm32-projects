#include "uart.h"

#define GPIOAEN  		(1U<<0)
#define UART2EN			(1U<<17)

#define CR1_TE 			(1U<<3) // CONTROL register 1.
#define CR1_RE 			(1U<<2) // enable RX through CR1.
#define CR1_UE 			(1U<<13)

#define SR_TXE 			(1U<<7) // to check if uart tx is empty
#define SR_RXNE 		(1U<<5)
#define CR1_RXNEIE		(1U<<5) // interupt for RXNE enable
#define DMA_MEM_INC		(1U<<10)
#define DMA_DIR_MEM_TO_PERIPH	(1U<<6)
#define DMA_CR_TCIE 	(1U<<4)
#define DMA_CR_EN		(1U<<0)
#define UART_CR3_DMAT	(1U<<7)

#define DMA1EN 			(1U<<21)
#define DMA_S_EN 		(1U<<0)
#define CHSEL4			(1U<<27)


#define SYS_FREQ 		16000000
#define APB1_CLK 		SYS_FREQ

#define UART_BAUDRATE 	115200

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_BD(uint32_t PeriphClk, uint32_t BaudRate);


void uart2_write(int ch);

int __io_putchar(int ch)
{
	uart2_write(ch);
	return ch;
}

void dma1_stream6_init(uint32_t src, uint32_t dst, uint32_t len)
{
	/* enable clock access to the DMA */
	RCC->AHB1ENR |= DMA1EN;

	/* disable DMA1 Stream6 */
	DMA1_Stream6->CR &=~DMA_S_EN;

	/* clear all the interrupt flags of Stream6*/
	DMA1->HIFCR |=(1U<<16);
	DMA1->HIFCR |=(1U<<18);
	DMA1->HIFCR |=(1U<<19);
	DMA1->HIFCR |=(1U<<21);

	/* set the destination buffer */
	DMA1_Stream6->PAR = dst;

	/* set the source buffer */
	DMA1_Stream6->M0AR = src;

	/* set length Number of data transfer items register */
	DMA1_Stream6->NDTR = len;

	/* Select Stream6 CH4. we clear register with this too. */
	DMA1_Stream6->CR = CHSEL4;

	/* we want the data to increment to where it is copied. Enable Memory Increment: */
	DMA1_Stream6->CR |= DMA_MEM_INC;

	/* Data will move from memory -> peripheral. Configure transfer direction*/
	DMA1_Stream6->CR |= DMA_DIR_MEM_TO_PERIPH;

	/* Enable DMA transfer complete interrupt */
	DMA1_Stream6->CR |= DMA_CR_TCIE;

	/*Enable Direct mode and disable FIFO */
	DMA1_Stream6->FCR = 0;

	/* Enable DMA1 Stream6 */
	DMA1_Stream6->CR |= DMA_CR_EN;

	/* Enable UART2 Transmitter DMA */
	USART2->CR3 |= UART_CR3_DMAT;

	/*DMA Interrupt enable in NVIC  */
	NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

void uart2_rxtx_init(void)
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

	// PA3 is the RX line of our USART2.
	GPIOA->MODER &=~(1U<<6);
	GPIOA->MODER |= (1U<<7);

	// Make the Alternate function configured to be for AF7.
	GPIOA->AFR[0] |= (1U<<12);
	GPIOA->AFR[0] |= (1U<<13);
	GPIOA->AFR[0] |= (1U<<14);
	GPIOA->AFR[0] &=~(1U<<15);

	/* Enable clock access to uart2. Must come BEFORE any USART2 write,
	   or the writes below land on an unclocked peripheral and vanish. */
	RCC->APB1ENR |= UART2EN;

	// configure the baudrate.
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	/* configure the transfer direction: transmitter AND receiver */
	USART2->CR1 = (CR1_TE | CR1_RE);

	// enable uart module
	USART2->CR1 |= CR1_UE;
}

void uart2_rxtx_interrupt_init(void)
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

	// PA3 is the RX line of our USART2.
	GPIOA->MODER &=~(1U<<6);
	GPIOA->MODER |= (1U<<7);

	// Make the Alternate function configured to be for AF7.
	GPIOA->AFR[0] |= (1U<<12);
	GPIOA->AFR[0] |= (1U<<13);
	GPIOA->AFR[0] |= (1U<<14);
	GPIOA->AFR[0] &=~(1U<<15);

	/* Enable clock access to uart2. Must come BEFORE any USART2 write,
	   or the writes below land on an unclocked peripheral and vanish. */
	RCC->APB1ENR |= UART2EN;

	// configure the baudrate.
	uart_set_baudrate(USART2, APB1_CLK, UART_BAUDRATE);

	/* configure the transfer direction: transmitter AND receiver */
	USART2->CR1 = (CR1_TE | CR1_RE);

	/* enable RXNE interrupt */
	USART2->CR1 |= CR1_RXNEIE;

	/*enable uart2 interrupt in NVIC */
	NVIC_EnableIRQ(USART2_IRQn);


	// enable uart module
	USART2->CR1 |= CR1_UE;
}

void uart2_write(int ch)
{
	// make sure the transmit data register is empty
	while(!(USART2->SR & SR_TXE)){} // When does this return true? i have no idea.

	// Write to transmit data register
	USART2->DR = (ch & 0xFF);
}

void uart2_tx_init(void)
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
	USART2->CR1 = (CR1_TE | CR1_RE); // Enables BOTH TX and RX.

	/* enable the uart module. Bit 13 is UE. Uart enable*/
	USART2->CR1 |= CR1_UE;
}

char uart2_read(void)
{
	/*Make sure the receive data register is NOT empty.*/
	while(!(USART2->SR & SR_RXNE)){}
	// Reads data
	return USART2->DR;
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate)
{
	USARTx->BRR = compute_uart_BD(PeriphClk, BaudRate);
}

static uint16_t compute_uart_BD(uint32_t PeriphClk, uint32_t BaudRate)
{
	return ((PeriphClk + (BaudRate/2U))/BaudRate);
}
