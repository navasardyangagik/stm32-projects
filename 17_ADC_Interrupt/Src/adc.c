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
#define CR1_EOCIE		(1U<<5)

void pa1_adc_interrupt_init(void)
{
	/* Enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/* Set the mode of PA1 to analog. */
	GPIOA->MODER |= (1U<<2);
	GPIOA->MODER |= (1U<<3);

	/* Enable clock access to adc */
	RCC->APB2ENR |= ADC1EN;

	/* enable for end of conversion for ADC1 */
	ADC1->CR1 |= CR1_EOCIE;

	/* enable ADC interrupts in the NVIC, so they can reach the CPU */
	NVIC_EnableIRQ(ADC_IRQn);

	/* Conversion sequence start */
	ADC1->SQR3 = ADC_CH1;
	/* Configure the conversion sequence length */
	ADC1->SQR1 = ADC_SEQ_LEN_1;

	/* enable the ADC module */
	ADC1->CR2 |= CR2_ADON;
}

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
	// there's no need for us here to wait for the flag. if there's an interrupt it means we can just get the result.
	/* read the converted result */
	return ADC1->DR;
}
