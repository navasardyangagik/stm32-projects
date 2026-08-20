#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "uart.h"
#include "adc.h"



uint32_t sensor_value;

int main(void)
{
	pa1_adc_interrupt_init();
	uart2_tx_init();
	start_conversion();

	while (1)
	{

	}
}

static void adc_callback(void)
{
	sensor_value = adc_read();
	printf("Sensor Value : %d \n\r", (int)sensor_value);
}

void ADC_IRQHandler(void)
{
	/* we need to check if the end of conversion flag is raised in the Status Register.  */
	if ((ADC1->SR & SR_EOC) != 0)
	{
		/* then we must clear it. We didn't have to do this for UART. */
		ADC1->SR &=~(SR_EOC);


		adc_callback();
	}

}
