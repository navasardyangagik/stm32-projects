#include <stdint.h>
#include "stm32f4xx.h"
#include "tim.h"


#define SR_CC1IF 	(1U<<1)

int timestamp = 0;
int main(void)
{
	tim2_output_compare();
	tim3_input_capture();

	while (1)
	{
		/* Wait until edge is captured */
		while(!(TIM3->SR & SR_CC1IF)){}

		/* Read the captured Value */
		timestamp = TIM3->CCR1;

		/* our signal source is going to be PA5. our goal is to get that output and feed it into PA6 to use that as the input.
		we connect the two using a jumper wire. */
	}
}
