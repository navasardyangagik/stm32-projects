#include <stdint.h>

#define PERIPH_BASE			(0x40000000UL) // Base number of where the periph. addresses begin according to data sheet
#define AHB1PERIPH_OFFSET	(0x00020000UL)
#define AHB1PERIPH_BASE		(PERIPH_BASE + AHB1PERIPH_OFFSET)
#define GPIOA_OFFSET 		(0x0000U) // 0x 0000 0000 amount of the zeros don't matter. preceeding zero's dont matter. Not active bits.

#define GPIOA_BASE			(AHB1PERIPH_BASE + GPIOA_OFFSET)

#define RCC_OFFSET 			(0x3800UL)
#define RCC_BASE 			(AHB1PERIPH_BASE + RCC_OFFSET)




#define GPIOAEN				(1U<<0) // (shift 1 at pos 0) 0b 0000 0000 0000 0000 0000 0000 0000 0001

#define PIN5 				(1U<<5)
#define LED_PIN 			PIN5

#define __IO volatile

typedef struct
{
  volatile uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
  volatile uint32_t DUMMY[4];
  volatile uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
} GPIO_TypeDef;

#define RCC		((RCC_TypeDef*)RCC_BASE)
#define GPIOA 	((GPIO_TypeDef*)GPIOA_BASE)

typedef struct
{
  volatile uint32_t DUMMY[12];
  volatile uint32_t AHB1ENR; 	/*!< RCC AHB1 peripheral clock register, Address offset: 0x30 */
} RCC_TypeDef;



int main(void)
{
	// 1. Enable clock access to GPIOA
	// 2. SET PA5 as output pin
	RCC->AHB1ENR |= GPIOAEN;

	GPIOA->MODER |= (1U<<10); // set bit 10 to 1
	GPIOA->MODER &=~(1U<<11); // set bit 11 to 0

	while (1)
	{
		GPIOA->ODR ^= LED_PIN; // Toggle operator ^ goes on and off. happens too quick though.
		for (int i=0; i < 100000; i++){}
	}
}
