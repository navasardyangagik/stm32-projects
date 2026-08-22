#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "i2c.h"

uint8_t buf[3];
uint32_t adc_T;

int main(void)
{
	/* turn on our BME280 sensor. Currently it's on sleep mode */
	I2C_Config();
	I2C_Start();
	I2C_Address_Write(0x76);
	I2C_Write(0xF4);
	I2C_Write(3U<<0);


	/* restart our I2C module to switch to read mode for data retrieval */
	I2C_Start();
	I2C_Address_Read(0x76);
	I2C_Read_Buffer(buf, 3);

	adc_T = buf[0]<<12 | buf[1]<<4 | buf[2]>>4;

	while (1){}
}
