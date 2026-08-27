#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "spi.h"
#include "uart.h"

uint8_t cmd = 0x9F;
uint8_t id[3];
uint8_t read_data[18];


static void spi1_transmit_helper(uint8_t hex)
{
	spi1_transmit(&hex, 1);
}

int main(void)
{
	spi_gpio_init(); // initialize our GPIOA pins to be used for SPI
	spi1_config(); // Initialize our SPI peripheral

	cs_enable();
	spi1_transmit(&cmd, 1);
	spi1_receive(id, 3);
	cs_disable();
//
//	/* enable writes */
//	cs_enable();
//	spi1_transmit_helper(0x06);
//	cs_disable();
//
//	/* page program + send out address and then data*/
//	cs_enable();
//	uint8_t data[18] = "Hello from Linux!";
//	spi1_transmit_helper(0x02); // Page program
//	spi1_transmit_helper(0x00); // Block 0 (64KB) beginning address
//	spi1_transmit_helper(0x00);
//	spi1_transmit_helper(0x00);
//	spi1_transmit(data, 18);
//	cs_disable();
//
//	uint8_t status;
//	do
//	{
//		cs_enable();
//		spi1_transmit_helper(0x05);
//		spi1_receive(&status, 1);
//		cs_disable();
//	} while (status & 0x01);

	/* read the data stored in our 0x00 address storage and printf out with UART */
	cs_enable();
	spi1_transmit_helper(0x03);
	spi1_transmit_helper(0x00);
	spi1_transmit_helper(0x00);
	spi1_transmit_helper(0x00);
	spi1_receive(read_data, 18);
	cs_disable();

	uart2_rxtx_init();

	printf("%s\n\t", read_data);

	while(1)
	{

	}
}
