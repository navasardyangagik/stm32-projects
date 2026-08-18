#include "stm32f4xx.h"
#include "adc.h"

#define GPIOAEN 		(1U<<0)
#define ADC1EN 			(1U<<8)
#define ADC_CH1 		(1U<<0)
#define ADC_SEQ_LEN_1	0x00
#define CR2_ADON 		(1U<<0)
#define CR2_SWSTART 	(1U<<30)
#define SR_EOC			(1U<<1)
#define CR2_CONT		(1U<<1)

// 0B 0000 0001

/*
	Lets say we have ADC configured with 3 channels
	ch2, ch3, ch5. And lets say for example this is the sampling order we desire
	first = ch5
	second = ch2
	third = ch3

	This means in SQ1, the first sequence would be the binary value 5.
	Then in SQ2 it would be the binary 2. And then binary 3.
*/

void pa1_adc_init(void)
{
	/* Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/* Set the mode of PA1 to analog. */
	GPIOA->MODER |= (1U<<2);
	GPIOA->MODER |= (1U<<3);



	/* Configure the adc module */

	/* Enable clock access to adc */
	RCC->APB2ENR |= ADC1EN;

	/* Conversion sequence start */
	ADC1->SQR3 = ADC_CH1;
	/* Configure the conversion sequence length */
	ADC1->SQR1 = ADC_SEQ_LEN_1;

	/* enable the ADC module */
	ADC1->CR2 |= CR2_ADON;
}

void start_conversion(void)
{
	/* enable continuous conversion */
	ADC1->CR2 |= CR2_CONT;
	/* start the adc conversion */
	ADC1->CR2 |= CR2_SWSTART;
}

uint32_t adc_read(void)
{
	/* wait for the conversion to be complete before we read */
	/* there's a flag that is RAISED when the conversion is complete. This is inside the STATUS REGISTER (SR) */
	while (!(ADC1->SR & SR_EOC)) {}

	/* read the converted result */
	return ADC1->DR;
}
