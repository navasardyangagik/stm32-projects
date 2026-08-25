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

#define SR_RXNE 		(1U<<5)
#define HISR_TCIF6 		(1U<<21)
#define HIFCR_CTCIF6	(1U<<21)

void uart2_tx_init(void);
char uart2_read(void);
void uart2_rxtx_init(void);
void uart2_rxtx_interrupt_init(void);
void dma1_stream6_init(uint32_t src, uint32_t dst, uint32_t len);


#endif /* UART_H_ */
