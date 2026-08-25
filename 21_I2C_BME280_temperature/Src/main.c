#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "i2c.h"
#include "uart.h"

uint8_t buf[3];
uint32_t adc_T;
int32_t temp_final;

int main(void)
{
	uart2_rxtx_init(); // use uart to get my temp data streamed to my computers terminal

	/* turn on our BME280 sensor. Currently it's on sleep mode */
	I2C_Config();
	BME_WriteReg(0xF4, (3U<<0 | 1U<<5));


	uint8_t calib[6];
	BME_ReadBuf(0x88, calib, 6);

	dig_T1 = (uint16_t)((calib[1] << 8 | calib[0]));
	dig_T2 = (int16_t)((calib[3] << 8 | calib[2]));
	dig_T3 = (int16_t)((calib[5] << 8 | calib[4]));

	while (1)
	{
		while (BME_ReadReg(0xF3) & (1U<<3)) {}

		BME_ReadBuf(0xFA, buf, 3);

		adc_T = buf[0]<<12 | buf[1]<<4 | buf[2]>>4;

		temp_final = BME280_compensate_T_int32((int32_t)adc_T);

		printf("Temperature: %ld.%02ld deg C%\r\n", temp_final / 100, temp_final % 100);
	}
}
