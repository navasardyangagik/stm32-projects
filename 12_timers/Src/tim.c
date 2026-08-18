#include "stm32f4xx.h"
#include "tim.h"

#define TIM2EN 		(1U<<0)
#define CR1_CEN 	(1U<<0)

void tim2_1hz_init(void)
{
	/* enable clock access to timer 2 through APB1 Bus */
	RCC->APB1ENR |= TIM2EN;
	/* set the prescaler value */
	TIM2->PSC = 1600 - 1; // 16 000 000 / 1 600 = 10 000
	/* set the audo reload value */
	TIM2->ARR = 10000 - 1; // 10 000 / 10 000 = 1
	/* clear the timer counter */
	TIM2->CNT = 0;
	/* enable the timer. it's okay to fully clear it*/
	TIM2->CR1 = CR1_CEN;
}
