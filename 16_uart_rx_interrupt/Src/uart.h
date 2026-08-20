/*
 * uart.h
 *
 *  Created on: Aug 17, 2026
 *      Author: dracule
 */

#ifndef UART_H_
#define UART_H_
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>

void uart2_tx_init(void);
char uart2_read(void);
void uart2_rxtx_init(void);
void uart2_rxtx_interrupt_init(void);
#define SR_RXNE 		(1U<<5)

#endif /* UART_H_ */
