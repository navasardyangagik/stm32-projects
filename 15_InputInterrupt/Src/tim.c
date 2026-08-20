#include "stm32f4xx.h"
#include "tim.h"

#define TIM2EN 		(1U<<0)
#define TIM3EN 		(1U<<1)
#define CR1_CEN 	(1U<<0)
#define OC_TOGGLE 	(1U<<4) | (1U<<5)
#define CCER_CC1E 	(1U<<0)
#define GPIOAEN 	(1U<<0)
#define CCMR1_CC1S 	(1U<<0)

void tim2_1hz_init(void)
{
	/* enable clock access to timer 2 through APB1 Bus */
	RCC->APB1ENR |= TIM2EN;
	/* set the prescaler value */
	TIM2->PSC = 1600 - 1; // 16 000 000 / 1 600 = 10 000
	/* set the auto reload value */
	TIM2->ARR = 10000 - 1; // 10 000 / 10 000 = 1
	/* clear the timer counter */
	TIM2->CNT = 0;
	/* enable the timer. it's okay to fully clear it*/
	TIM2->CR1 = CR1_CEN;
}


void tim2_output_compare(void)
{
	/* enable clock access to GPIOA */
	RCC->AHB1ENR |= GPIOAEN;
	/* set PA5 mode to alternate function */
	GPIOA->MODER &=~(1U<<10);
	GPIOA->MODER |= (1U<<11);
	/* set alternate function type to TIM2 CH1 its in AF01*/
	GPIOA->AFR[0] |= (1U<<20);
	GPIOA->AFR[0] &=~(1U<<21);
	GPIOA->AFR[0] &=~(1U<<22);
	GPIOA->AFR[0] &=~(1U<<23);
	/* enable clock access to timer 2 through APB1 Bus */
	RCC->APB1ENR |= TIM2EN;
	/* set the prescaler value */
	TIM2->PSC = 1600 - 1; // 16 000 000 / 1 600 = 10 000
	/* set the auto reload value */
	TIM2->ARR = 10000 - 1; // 10 000 / 10 000 = 1

	/* Set output compare toggle mode */
	TIM2->CCMR1 = OC_TOGGLE;

	/* Enable tim2 ch1 in compare mode */
	TIM2->CCER |= CCER_CC1E;



	/* clear the timer counter */
	TIM2->CNT = 0;
	/* enable the timer. it's okay to fully clear it*/
	TIM2->CR1 = CR1_CEN;
}

void tim3_input_capture(void)
{
	/* our goal is to capture pa5 input through timer3. */
	/* we want to figure out which pin we will use as an input pin */
	/* enable clock access to GPIOA */
	RCC->AHB1ENR |= GPIOAEN;
	/* set PA6 to alternate function mode (AF02)*/
	GPIOA->MODER &=~(1U<<12);
	GPIOA->MODER |= (1U<<13);

	GPIOA->AFR[0] &=~(1U<<24);
	GPIOA->AFR[0] |= (1U<<25);
	GPIOA->AFR[0] &=~(1U<<26);
	GPIOA->AFR[0] &=~(1U<<27);

	/* enable clock access to tim3 */
	RCC->APB1ENR |= TIM3EN;

	/* set Prescaler and period. */
	TIM3->PSC = 16000 - 1; // dividing our clock : 16 000 000 / 16 000

	/* set CH1's capture mode to input capture.*/
	TIM3->CCMR1 = CCMR1_CC1S;

	/* further specify that we want it to capture at rising edge*/
	TIM3->CCER |= CCER_CC1E;

	/* enable TIM3 */
	TIM3->CR1 |= CR1_CEN;
}
