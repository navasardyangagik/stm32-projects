#include "i2c.h"

#define I2C1EN 				(1U<<21)
#define GPIOBEN				(1U<<1)

/*
The following is the required sequence in controller mode.
• Program the peripheral input clock in I2C_CR2 register in order to generate correct
timings
• Configure the clock control registers
• Configure the rise time register
• Program the I2C_CR1 register to enable the peripheral
• Set the START bit in the I2C_CR1 register to generate a Start condition
• The peripheral input clock frequency must be at least:
• 2 MHz in Sm mode
• 4 MHz in Fm mode
 */


void I2C_Config(void)
{
	/* enable clock for the port we will use, Port B. (Pin 8 & 9)*/
	RCC->AHB1ENR |= GPIOBEN;

	/* enable I2C1 clock RCC */
	RCC->APB1ENR |= (1U<<21);

	/* Find the connected GPIO pin and set it to its alternate peripheral mode */
	GPIOB->MODER &=~(1U<<16);
	GPIOB->MODER |= (1U<<17);

	GPIOB->MODER &=~(1U<<18);
	GPIOB->MODER |= (1U<<19);
	
	/* Set the output type register to open drain. This is because we don't want to use the PMOS */
	GPIOB->OTYPER |= ((1U<<8) | (1U<<9));

	/* For pins 8 and 9 enable pull up register (adds a resistor internally)  */
	GPIOB->PUPDR |= (1U<<16);
	GPIOB->PUPDR |= (1U<<18);

	/* Set alternate function type in AFRH to AF04 */
	GPIOB->AFR[1] |= (4U<<0);
	GPIOB->AFR[1] |= (4U<<4);

	/* reset the I2C. 15th bit in CR1. SWRST. */
	I2C1->CR1 |= (1U<<15);
	I2C1->CR1 &=~(1U<<15);

	/* Program the peripheral input clock in I2C_CR2 register in order to get correct timings */
	I2C1->CR2 = (16<<0); // placing the number 16 to represent 16 Mhz for our clock.

	/* now we configure the clock control register. we calculate the CCR using the T_high or T_low formula. values found in data sheet*/
	I2C1->CCR = 80; // T_high = CCR * T_PCLK1 | T_SCL = 2 * CCR * T_PCLK1. At 16Mhz T_PCLK = 62.5 ns (1 period) |  CCR = 10,000 ns / ( 2 * 62.5 ns ) = 160 | 160/2 = 80

	/* configure the rise time. which is max 1000 ns according to our data sheet */
	I2C1->TRISE = 17; // T_PCLK1 = 62.5 ns for one period. Formula is T_rSCL / T_PCLK1 + 1. | 1,000 ns / 62.5 ns + 1 = 17

	/* enable the peripheral */
	I2C1->CR1 |= (1U<<0);
	
}

void I2C_Start(void)
{
	/* First we set the start start bit and wait for the SB bit to be set, indicating the start condition is generated.  */
	I2C1->CR1 |= (1U<<8);

	/* Then we wait for the status bit to be set in the SR */
	while (!(I2C1->SR1 & (1U<<0))){}
}

void I2C_Write(uint8_t data) 
{
	/* wait for the TXE bit to be set. (bit 7 in SR1). This indicated the data register is empty */
	while (!(I2C1->SR1 & (1U<<7)));

	/* send data to the data register */
	I2C1->DR = data;

	/* wait for the BTF (Bit 2) in the SR1 to set. This indicated the end of the LAST DATA transmission. */
	while(!(I2C1->SR1 & (1U<<2)));
}

void I2C_Address_Write(uint8_t address)
{
	/* create this temporary variable to read SR1 SR2 to eventually. */
	volatile uint32_t tmp;

	/* Send the slave address to the DR register. The shift leaves bit 0 clear, which is write mode. */
	I2C1->DR = (address << 1);

	/* we need to wait for the addr bit to be set. Plus we read SR1 here. */ 
	while(!(I2C1->SR1 & (1U<<1)));

	/* Finally read SR2 to fully clear ADDR bit. */
	tmp = I2C1->SR2;
}

void I2C_Address_Read(uint8_t address)
{
	/* Send the slave address to the DR register. Bit 0 set means read mode. */
	I2C1->DR = (address << 1) | 1U;

	/* Wait for the addr bit to be set. This reads SR1, step 1 of the two-step clear. */
	while(!(I2C1->SR1 & (1U<<1)));

	/* NOTE: ADDR is deliberately left SET here. While ADDR is set the peripheral
	 * holds SCL low, so the bus simply waits for us. I2C_Read() finishes the clear,
	 * because clearing ADDR is what starts the data flowing, and the ACK policy
	 * has to be decided before that happens. */
}

uint8_t I2C_Read(void)
{
	/* create this temporary variable to read SR2 and finish clearing ADDR. */
	volatile uint32_t tmp;

	/* Single byte read, so this one byte is also the last byte. Clear ACK now so the
	 * hardware already knows to NACK it. Must happen before ADDR is cleared, because
	 * clearing ADDR releases SCL and the byte immediately starts arriving. */
	I2C1->CR1 &=~(1U<<10);

	/* Finish clearing ADDR. SR1 was read in I2C_Address_Read, so SR2 completes it. */
	tmp = I2C1->SR2;

	/* Pre-arm the STOP. It goes out on the wire after the byte finishes arriving. */
	I2C1->CR1 |= (1U<<9);

	/* Wait for RXNE, meaning a whole byte has landed in the data register. */
	while(!(I2C1->SR1 & (1U<<6)));

	/* Reading DR both retrieves the byte and clears RXNE. */
	return (uint8_t)I2C1->DR;
}

void I2C_Stop(void)
{
	/* bit 9 stops the protocl in the control register 1 */
	I2C1->CR1 |= (1U<<9);
}

void I2C_WriteMulti (uint8_t *data, uint8_t size)
{
	/* Wait for the TXE bit to be set, indicating data register is empty. */
	while (!(I2C1->SR1 & (1U<<7)));

	/* Send data to the data register */
	while (size)
	{
		while (!(I2C1->SR1 & (1U<<7)));
		I2C1->DR = *data++;
		size--;
	}

	/* Once the data transfer is complete, wait for the BTF to be set indicating the end of the last transmission. */
	while(!(I2C1->SR1 & (1U<<2)));
}
