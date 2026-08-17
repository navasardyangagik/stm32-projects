// Where is the led connected? User Guide Says PA5
// Port: A
// Pin: 5
#include <stdint.h>

#define PERIPH_BASE			(0x40000000UL) // Base number of where the periph. addresses begin according to data sheet
#define AHB1PERIPH_OFFSET	(0x00020000UL)
#define AHB1PERIPH_BASE		(PERIPH_BASE + AHB1PERIPH_OFFSET)
#define GPIOA_OFFSET 		(0x0000U) // 0x 0000 0000 amount of the zeros don't matter. preceeding zero's dont matter. Not active bits.

#define GPIOA_BASE			(AHB1PERIPH_BASE + GPIOA_OFFSET)

#define RCC_OFFSET 			(0x3800UL)
#define RCC_BASE 			(AHB1PERIPH_BASE + RCC_OFFSET)

#define AHB1EN_R_OFFSET 	(0x30UL) // ? for later: why is it not called RCC enable as that gives the clock to AHB1, i thought we are enabling rcc?
#define RCC_AHB1EN_R 		(*(volatile unsigned int *)(RCC_BASE + AHB1EN_R_OFFSET))

#define MODE_R_OFFSET 		(0x00UL)
#define GPIOA_MODE_R 		(*(volatile unsigned int *)(GPIOA_BASE + MODE_R_OFFSET))


#define OD_R_OFFSET 		(0x14UL)
#define GPIOA_OD_R 			(*(volatile unsigned int *)(GPIOA_BASE + OD_R_OFFSET))


#define GPIOAEN				(1U<<0) // (shift 1 at pos 0) 0b 0000 0000 0000 0000 0000 0000 0000 0001

#define PIN5 				(1U<<5)
#define LED_PIN 			PIN5

// Setting bit 10 to 1 and bit 11 to zero. 1U<<10 will set bit 10 to 1.
// we use an inversion &=~(1U<<11) to set that 11th bit to zero. Tilda inverts. &= and operation with CURRENT content of register.

/*
rcc_ahbenr1 reg = 0b 0000 0000 0000 0000 1000 1111 0000 0000

rcc_ahbenr1 reg = 0b 0000 0000 0000 0000 0000 0000 0000 0001

If we just apply an equal sign it CLEARS everything and we NEVER want that. We use OR operator.
*/

int main(void)
{
	// 1. Enable clock access to GPIOA
	// 2. SET PA5 as output pin
	RCC_AHB1EN_R |= GPIOAEN;
	GPIOA_MODE_R |= (1U<<10); // set bit 10 to 1
	GPIOA_MODE_R &=~(1U<<11); // set bit 11 to 0

	while (1)
	{
		// 3. Set PA5 high
		//GPIOA_OD_R |= LED_PIN;

		// 4. experiment. toggle PA5
		GPIOA_OD_R ^= LED_PIN; // Toggle operator ^ goes on and off. happens too quick though.
		for (int i=0; i < 100000; i++){}
	}
}
