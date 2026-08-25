#include "i2c.h"

#define I2C1EN 				(1U<<21)
#define GPIOBEN				(1U<<1)


#define I2C_100KHZ 				80
#define SD_MODE_MAX_RISE_TIME 	17
#define CR1_PE 					(1U<<0)

#define SR2_BUSY 				(1U<<1)
#define CR1_START 				(1U<<8)
#define SR1_SB 					(1U<<0)
#define SR1_ADDR 				(1U<<1)
#define SR1_TXE 				(1U<<7)
#define CR1_STOP 				(1U<<9)
#define SR1_RXNE 				(1U<<6)
#define CR1_ACK 				(1U<<10)
#define SR1_BTF 				(1U<<2)

void I2C1_init(void)
{
	/* enable clock for the port we will use, Port B. (Pin 8 & 9)*/
	RCC->AHB1ENR |= GPIOBEN;

	/* Find the connected GPIO pin and set it to its alternate peripheral mode */
	GPIOB->MODER &=~(1U<<16);
	GPIOB->MODER |= (1U<<17);

	GPIOB->MODER &=~(1U<<18);
	GPIOB->MODER |= (1U<<19);


	/* set PB8 and PB9 output type to open drain */
	GPIOB->OTYPER |= (1U<<8);
	GPIOB->OTYPER |= (1U<<9);

	/* enable pull up for PB8 and PB9 */
	GPIOB->PUPDR |= (1U<<16);
	GPIOB->PUPDR &=~(1U<<17);

	GPIOB->PUPDR |= (1U<<18);
	GPIOB->PUPDR &=~(1U<<19);

	/* Set alternate function type in AFRH to AF04 (I2C1) for PB8 and PB9 */
	GPIOB->AFR[1] |= (4U<<0);
	GPIOB->AFR[1] |= (4U<<4);

	/* enable clock access to I2C1 */
	RCC->APB1ENR |= I2C1EN;

	/* enter reset mode. */
	I2C1->CR1 |= (1U<<15);

	/* come out of reset mode */
	I2C1->CR1 &=~(1U<<15);

	/* set peripheral clock frequency */
	I2C1->CR2 = (1U<<4); // 16 MHz.

	/* set CCR (Control Clock Register)	 */
	I2C1->CCR = I2C_100KHZ;

	/* set rise time */
	I2C1->TRISE = SD_MODE_MAX_RISE_TIME;

	/* enable I2C */
	I2C1->CR1 |= CR1_PE;
}

void I2C1_byteRead(char saddr, char maddr, char* data)
{
	volatile int tmp;
	/* have to wait and make sure our I2C isn't busy. */
	while (I2C1->SR2 & SR2_BUSY) {}

	/* Generate start */
	I2C1->CR1 |= CR1_START;

	/* wait for the start flag to be set */
	while (!(I2C1->SR1 & (SR1_SB))){} // While the start bit is NOT set do NOTHING

	/* now we must transmit the address and say we wish to write to the slave */
	I2C1->DR = saddr << 1;

	/* wait until addr flag is set */
	while (!(I2C1->SR1 & (SR1_ADDR))){}

	/* clear addr flag */
	tmp = I2C1->SR2;
	(void)tmp;

	/* send memory address */
	I2C1->DR = maddr;

	/* wait until data register is empty */
	while (!(I2C1->SR1 & (SR1_TXE))) {}

	/* generate a re start condition */
	I2C1->CR1 |= CR1_START;

	/* wait until the start flag is set */
	while (!(I2C1->SR1 & (SR1_SB))){}

	/* Transmit slave address + Read */
	I2C1->DR = saddr << 1 | 1;

	/* wait until addr flag is set */
	while (!(I2C1->SR1 & (SR1_ADDR))){}

	/* disable acknowledge */
	I2C1->CR1 &=~CR1_ACK;

	/* clear addr flag */
	tmp = I2C1->SR2;
	(void)tmp;

	/* Generate stop after data received */
	I2C1->CR1 |= CR1_STOP;

	/* wait until RXNE flag is set */
	while (!(I2C1->SR1 & (SR1_RXNE))){}

	/* read the data register and store it */
	*data++ = I2C1->DR;

}

void I2C1_burstWrite(char saddr, char maddr, int n, char *data)
{
	volatile int tmp;

	while (I2C1->SR2 & SR2_BUSY) {} // make sure our bus isn't busy.

	/* Generate start */
	I2C1->CR1 |= CR1_START;

	/* wait for the start flag to be set */
	while (!(I2C1->SR1 & (SR1_SB))){}

	/* send slave address */
	I2C1->DR = saddr << 1;

	/* wait for addr flag to be set */
	while(!(I2C1->SR1 & (SR1_ADDR))){}

	/* clear the addr flag by reading SR2, we just read SR1. */
	tmp = I2C1->SR2;
	(void)tmp;

	/* Wait until transmitter is empty */
	while(!(I2C1->SR1 & (SR1_TXE))){} // done for best practice.

	/* send memory address. write mode */
	I2C1->DR = maddr;

	/* enable the ACK bit */
	I2C1->CR1 |= CR1_ACK;

	for (int i = 0; i < n; i++)
	{
		/* wait until data register is empty */
		while(!(I2C1->SR1 & (SR1_TXE))){}

		I2C1->DR = *data++;
	}

	while (!(I2C1->SR1 & (SR1_BTF))){}

	I2C1->CR1 &=~CR1_ACK;
	I2C1->CR1 |= CR1_STOP;
}

void I2C1_burstRead(char saddr, char maddr, int n, char *data)
{
	volatile int tmp;

	while (I2C1->SR2 & SR2_BUSY) {} // make sure our bus isn't busy.

	/* Generate start */
	I2C1->CR1 |= CR1_START;

	/* wait for the start flag to be set */
	while (!(I2C1->SR1 & (SR1_SB))){}

	/* send slave address */
	I2C1->DR = saddr << 1;

	/* wait for addr flag to be set */
	while(!(I2C1->SR1 & (SR1_ADDR))){}

	/* clear the addr flag by reading SR2, we just read SR1. */
	tmp = I2C1->SR2;
	(void)tmp;

	/* Wait until transmitter is empty */
	while(!(I2C1->SR1 & (SR1_TXE))){} // done for best practice.

	/* send memory address. write mode still */
	I2C1->DR = maddr;

	/* Wait until transmitter is empty */
	while(!(I2C1->SR1 & (SR1_TXE))){} // done for best practice.

	/* generate a re start condition */
	I2C1->CR1 |= CR1_START;

	/* wait until the start flag is set */
	while (!(I2C1->SR1 & (SR1_SB))){}

	I2C1->DR = saddr << 1 | 1; // send slave address in read mode

	/* wait for addr flag to be set */
	while (!(I2C1->SR1 & (SR1_ADDR))){}

	/* clear addr flag */
	tmp = I2C1->SR2;
	(void)tmp;

	/* enable the ACK bit */
	I2C1->CR1 |= CR1_ACK;

	while (n > 0)
	{
		if (n == 1)
		{
			/* pre-arm our stop condition after we get our data */
			I2C1->CR1 &=~CR1_ACK; // disable ACK to send NACK on next send after we get our data.
			I2C1->CR1 |= CR1_STOP;


			/* wait until RXNE flag is set */
			while (!(I2C1->SR1 & SR1_RXNE)){}

			/* get our data */
			*data++ = I2C1->DR;
			break;
		}
		else
		{
			/* wait until RXNE flag is set */
			while (!(I2C1->SR1 & SR1_RXNE)){}

			*data++ = I2C1->DR; // get data as usual and increment the pointer.
			n--;
		}
	}
}
