#include "spi.h"


#define SPIEN 			(1U<<12)
#define GPIOAEN			(1U<<0)


// PA5 -> CLK
// PA6 -> MISO
// PA7 -> MOSI

// PA9 -> Slave Select


void spi_gpio_init(void)
{
	/* enable GPIOA clock */
	RCC->AHB1ENR |= GPIOAEN;

	/* enable alternate register mode for pins PA5, PA6, PA7 */
	/* PA5 */
	GPIOA->MODER &= ~(1U<<10);
	GPIOA->MODER |= (1U<<11);

	/* PA6 */
	GPIOA->MODER &= ~(1U<<12);
	GPIOA->MODER |= (1U<<13);

	/* PA7 */
	GPIOA->MODER &= ~(1U<<14);
	GPIOA->MODER |= (1U<<15);


	/* Set PA9 as OUTPUT pin*/
	GPIOA->MODER |= (1U<<17);
	GPIOA->MODER &= ~(1U<<19);

	/* set the alternate function type to SPI (AF05) for pins 5, 6, 7 */
	GPIOA->AFR[0] |= (5U<<20);
	GPIOA->AFR[0] |= (5U<<24);
	GPIOA->AFR[0] |= (5U<<28);
}

void spi1_config(void)
{
	/* enable clock access to SPI1 Peripheral on APB2 bus */
	RCC->APB2ENR |= SPIEN;

	/* set clock to fPCLK / 4 */
	SPI1->CR1 |= (1U<<3);
	SPI1->CR1 &= ~(1U<<4);
	SPI1->CR1 &= ~(1U<<5);

	/* now we set the clock polarity and phase to 1. */
	SPI1->CR1 |= (1U<<0);
	SPI1->CR1 |= (1U<<1);

	/* now we must enable full duplex for simultaneous 2-way communication RXONLY */
	SPI1->CR1 &=~ (1U<<10);

	/* Set MSB first */
	SPI1->CR1 &=~ (1U<<7);

	/* set our SPI to be in master mode. Bit 2 is MSTR */
	SPI1->CR1 |= (1U<<2);

	/* set the data size. 8 bit mode. DFF bits.*/
	SPI1->CR1 &=~ (1U<<11);

	/* set SSI and SSM bit to 1. */
	SPI1->CR1 |= (1U<<8);
	SPI1->CR1 |= (1U<<9);

	/* enable SPI module */
	SPI1->CR1 |= (1U<<6);
}

void spi1_transmit(uint8_t *data, uint32_t size)
{
	uint32_t i = 0;
	uint8_t temp;

	while (i < size)
	{
		/* wait until the TXE bit is set in the SR */
		while (!(SPI1->SR & (1U<<1)));

		/* load in our data for transmission */
		SPI1->DR = data[i];
		i++;
	}
	/* wait until TXE is set */
	while (!(SPI1->SR & (1U<<1)));

	/* wait for busy flag to be set */
	while ((SPI1->SR & (1U<<7)));

	/* clear the overrun flag OVR by reading the data register and status register */
	temp = SPI1->DR;
	temp = SPI1->SR;
}

void spi1_receive(uint8 *data, uint32_t size)
{
	while (size)
	{
		/* send dummy data */
		SPI1->DR = 0;

		/* wait for the RXNE */
		while (!(SPI1->SR & (1U<<0)));

		/* now we can read the data */
		*data++ = SPI1->DR;
		size--;
	}
}

void cs_enable(void)
{
	/* we need to set the pin low by accessing GPIO A ODR pin 9.*/
	GPIOA->ODR &=~ (1U<<9);
}

void cs_disable(void)
{
	/* we need to set the pin HIGH by accessing GPIO A ODR pin 9.*/
	GPIOA->ODR |= (1U<<9);
}
