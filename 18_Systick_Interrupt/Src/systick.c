#include "stm32f4xx.h"

#define SYSTICK_LOAD_VAL 		16000 // This is what we wish to load to delay by 1 ms
#define CTRL_ENABLE 			(1U<<0)
#define CTRL_CLKSRC 			(1U<<2)
#define CTRL_COUNTFLAG 			(1U<<16)
#define CTRL_TICKINT			(1U<<1)


#define ONE_SEC_LOAD			16000000 // based on the default freq of our system

void systickDelayMs(int delay)
{
	/* configure systick in our .h file the structure is called SysTick*/
	SysTick->LOAD = SYSTICK_LOAD_VAL - 1;

	/* clear systick current value register */
	SysTick->VAL = 0;

	/* enable systick & select internal clock source */
	SysTick->CTRL = CTRL_ENABLE | CTRL_CLKSRC;

	for(int i = 0; i < delay; i++)
	{
		/* Wait until the COUNTFLAG is set */

		while((SysTick->CTRL & CTRL_COUNTFLAG) == 0) {}


	}
	SysTick->CTRL = 0;
}

void systick_1hz_interrupt(void)
{
	/* reload with number of clocks per second */
	SysTick->LOAD = ONE_SEC_LOAD -1; // -1 because we count from zero.

	/* clear systick current value register */
	SysTick->VAL = 0;

	/* enable systick & select internal clock source */
	SysTick->CTRL = CTRL_ENABLE | CTRL_CLKSRC;

	/* enable systick interrupt */
	SysTick->CTRL |= CTRL_TICKINT;

	// WE DONT NEED TO ENABLE SYSTICK IN THE NVIC...
}
