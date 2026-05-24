// Where is the led connected?
// Port: A
// Pin: 5
#include <stdint.h>

#define PERIPH_BASE 	(0x40000000UL)
#define RCC_BASE        (0x40023800UL)
#define GPIOA_BASE      (0x40020000UL)

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x14))

void delay(volatile uint32_t count) {
    while(count--);
}

int main(void)
{
    RCC_AHB1ENR |= (1 << 0);
    // according to data sheet, MODER uses 2 bits PER pin
    // Since 2 bits can represent the four states a GPIO can be in
    GPIOA_MODER |= (1 << 10);
    while(1)
    {
        GPIOA_ODR |= (1 << 5); // LED ON
        delay((volatile uint32_t)(5000000)); // WAIT
        GPIOA_ODR &= ~(1 << 5); // LED OFF
        delay((volatile uint32_t)(5000000)); // WAIT
    }
}
