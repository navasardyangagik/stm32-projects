#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "i2c.h"
#include "systick.h"

#define SADDR			(0x68) // Obtained from WHO_AM_I reset value
#define PWR_MGMT_1 		(0x6B)
#define CONFIG 			(0x1A)
#define SMPLRT_DIV 		(0x19)
#define GYRO_CONFIG 	(0x1B)
#define ACCEL_CONFIG 	(0x1C)

void MPU_WriteReg(char reg, char value)
{
	I2C1_burstWrite(SADDR, reg, 1, &value);
}

char whoami;
int16_t accel_values[3];

int main(void)
{
	I2C1_init(); // initialize our i2c module

	MPU_WriteReg(PWR_MGMT_1, (1U<<7)); // write to our device PWR_MGMT_1 register to reset

	/* sleep for 100 ms to ensure reset went forward */
	systickDelayMs(100);

	MPU_WriteReg(PWR_MGMT_1, (1U<<0));

	MPU_WriteReg(CONFIG, (3U<<0)); // Hz frequency config

	MPU_WriteReg(SMPLRT_DIV, (9U<<0)); // sample rate

	MPU_WriteReg(GYRO_CONFIG, (0U<<3));

	MPU_WriteReg(ACCEL_CONFIG, (0U<<3));

	/* now we finally read */
	I2C1_burstRead(SADDR, 0x75, 1, &whoami); // we get 68




	while(1)
	{
		/* burst read the accelerations */
		char unshifted_accels[6];

		I2C1_burstRead(SADDR, 0x3B, 6, unshifted_accels);

		accel_values[0] = ((unshifted_accels[0] << 8) | unshifted_accels[1]);
		accel_values[1] = ((unshifted_accels[2] << 8) | unshifted_accels[3]);
		accel_values[2] = ((unshifted_accels[4] << 8) | unshifted_accels[5]);

	}
}
