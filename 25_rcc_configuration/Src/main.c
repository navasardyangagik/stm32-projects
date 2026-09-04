#include "stm32f4xx.h"
#include "rcc.h"
#include "tim.h"

#define GPIOAEN			(1U<<0)
#define PIN5			(1U<<5)

#define LED				PIN5

int main(void)
{
	// enable GPIOA Pin 5 as output
	RCC->AHB1ENR |= GPIOAEN;
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &= ~(1U<<11);

	// bring the system clock up to 100 MHz off HSE + PLL
	rcc_100mhz_init();

	/* the system clock speed should now be 100 MHz. Let us prove it. */
	tim2_1hz_init();
	while(1)
	{
		while (!(TIM2->SR & SR_UIF)){}
		TIM2->SR &= ~(SR_UIF);
		GPIOA->ODR ^= LED;
	}
}
