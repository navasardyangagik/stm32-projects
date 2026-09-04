#include "stm32f4xx.h"
#include "rcc.h"

/* M N P values
 * M = 4
 * N = 100
 * P = 2
 *
 * Prescaler values
 * HPRE & PPRE2 = 1 | yields us 100 MHz which is valid for that bus
 * PPRE1 = 2 | yields 50 Mhz. Valid for that bus.
 *
 * We will require some additional latency. 3.6 volt range 100 MHz requires 3 WS. (4 CPU cycles)
 *
 * */

#define PWREN			(1U<<28)

#define CR_HSEBYP		(1U<<18)
#define CR_HSEON		(1U<<16)
#define CR_HSERDY		(1U<<17)
#define CR_PLLON		(1U<<24)
#define CR_PLLRDY		(1U<<25)

#define VOS_MASK		(3U<<14)
#define VOS_SCALE1		(3U<<14) // 0b11 is scale 1. Needed for 100 MHz.

#define LATENCY_MASK	(15U<<0)
#define LATENCY_3WS		(3U<<0)

#define PLLSRC_HSE		(1U<<22)

#define PLLM_MASK		(63U<<0)
#define PLLN_MASK		(511U<<6)
#define PLLP_MASK		(3U<<16)
#define PLLQ_MASK		(15U<<24)

#define PLLM_4			(4U<<0)
#define PLLN_100		(100U<<6)
#define PLLP_DIV2		(0U<<16) // PLLP is an encoding. 00 = /2
#define PLLQ_4			(4U<<24) // unused, no USB, but must be legal

#define HPRE_MASK		(15U<<4)
#define PPRE1_MASK		(7U<<10)
#define PPRE2_MASK		(7U<<13)

#define HPRE_DIV1		(0U<<4)
#define PPRE1_DIV2		(4U<<10) // 0b100 = /2
#define PPRE2_DIV1		(0U<<13)

#define SW_MASK			(3U<<0)
#define SW_PLL			(2U<<0)
#define SWS_MASK		(3U<<2)
#define SWS_PLL			(2U<<2)

void rcc_100mhz_init(void)
{
	// Set HSE Bypass bit to 1 indicating we wish to use external clock.
	RCC->CR |= CR_HSEBYP;

	// Enable high speed external clock
	RCC->CR |= CR_HSEON;

	// Wait for HSE read to be set.
	while(!(RCC->CR & CR_HSERDY));

	// prepare chip for 100 MHz. Enable PWR peripheral clock
	RCC->APB1ENR |= PWREN;

	// Set VOS to the derived value.
	PWR->CR &= ~VOS_MASK; // first clear bits 14 & 15
	PWR->CR |= VOS_SCALE1;

	// Configure our latency for our wait states. 3 WS (4 CPU cycles) is needed
	FLASH->ACR &= ~LATENCY_MASK;
	FLASH->ACR |= LATENCY_3WS;

	// Configure the PLL, PLL off. PLL source is our HSE
	RCC->PLLCFGR |= PLLSRC_HSE;

	// Configure our M  N P values. CLEAR first then set.
	RCC->PLLCFGR &= ~PLLM_MASK; // M
	RCC->PLLCFGR &= ~PLLN_MASK; // N
	RCC->PLLCFGR &= ~PLLP_MASK; // P
	RCC->PLLCFGR &= ~PLLQ_MASK; // Q

	RCC->PLLCFGR |= PLLM_4; // M
	RCC->PLLCFGR |= PLLN_100; // N
	RCC->PLLCFGR |= PLLP_DIV2; // Here P is already 0, but we explicitly show.
	RCC->PLLCFGR |= PLLQ_4; // Q

	// Turn the PLL on finally
	RCC->CR |= CR_PLLON;

	// wait for PLL to be ready
	while(!(RCC->CR & CR_PLLRDY));

	// set the bus prescalers (dividers). First HPRE
	RCC->CFGR &= ~HPRE_MASK;
	RCC->CFGR |= HPRE_DIV1; // no division

	// PPRE1 & PPRE2
	RCC->CFGR &= ~PPRE1_MASK;
	RCC->CFGR |= PPRE1_DIV2;

	RCC->CFGR &= ~PPRE2_MASK;
	RCC->CFGR |= PPRE2_DIV1; // no division

	// Set our system clock switch to PLL as our system clock.
	RCC->CFGR &= ~SW_MASK;
	RCC->CFGR |= SW_PLL;

	// wait for system clock switch status. Did the status LEAVE HSI -> PLL?
	while ((RCC->CFGR & SWS_MASK) != SWS_PLL);
}
