/* We want an interrupt to occur whenever we press our push button. PC13. And interrupt when a falling edge is detected. */

#include "exti.h"

#define GPIOCEN 			(1U<<2)
#define SYSCFGEN			(1U<<14)


void pc13_exti_init(void)
{
	/* recommended to disable global interrupt before we configure interrupt. Sometimes skipped. Good practice */
	__disable_irq();

	/* enable clock accesss for gpio C */
	RCC->AHB1ENR |= GPIOCEN;

	/* explicitly set PC13 as an input pin. though it is by default. */
	GPIOC->MODER &=~(1U<<26);
	GPIOC->MODER &=~(1U<<27);

	/* enable clock access to SYSCFG  */
	RCC->APB2ENR |= SYSCFGEN;

	/* select PORTC for EXTI13 */
	SYSCFG->EXTICR[3] |= (1U<<5);

	/* unmask EXTI13. simply means uncover. */
	EXTI->IMR |= (1U<<13);

	/* Select falling edge trigger */
	EXTI->FTSR |= (1U<<13);

	/* enable EXTI13 line in NVIC. we do this by using a function included in our cortex.*/
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	/* enable global interrupt */
	__enable_irq();
}
