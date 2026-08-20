/*
 * tim.h
 *
 *  Created on: Aug 18, 2026
 *      Author: dracule
 */

#ifndef TIM_H_
#define TIM_H_

void tim2_1hz_init(void);
void tim2_output_compare(void);
void tim3_input_capture(void);

#define SR_UIF 		(1U<<0)

#endif /* TIM_H_ */
