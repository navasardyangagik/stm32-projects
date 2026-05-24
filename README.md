# Embedded Systems

Working with the STM32F411RE (ARM Cortex-M4, 100MHz max, 512KB flash, 128KB SRAM) on the NUCLEO-F411RE development board. All projects are bare-metal. There's no RTOS (real-time operating system), no HAL, no Arduino, no libraries. Direct register manipulation using the reference manual and datasheet.

## Toolchain

- Compiler: arm-none-eabi-gcc
- Debugger/Flasher: OpenOCD over SWD
- Build system: Make
- Editor: Neovim

## Projects

### GPIO
Configuring pins as input/output, driving LEDs, reading buttons. Direct RCC clock enable and MODER/ODR register manipulation.

### Timers
Hardware timer configuration using prescaler and auto-reload registers. Exact timing without cycle-wasting delay loops.

### Interrupts
Interrupt service routines, NVIC configuration, timer interrupts for precise periodic execution.

### UART
Serial communication driver from scratch. Baud rate calculation, TX/RX register configuration, interrupt-driven receive.

### SPI
High speed synchronous serial communication. Master mode configuration, clock polarity and phase settings.

### I2C
Two wire communication protocol. Addressing, start/stop conditions, interfacing with sensors.

### DMA
Direct memory access for peripheral data transfer without CPU involvement.

### ADC
Analog to digital conversion. Reading sensor voltages, conversion modes, DMA triggered sampling.

## What I hope to accomplish:
I'd like to work closer with hardware as I've been more interested in low level programming. I hope to grasp all of the low level concepts discussed here and be able to work on any hardware project I desire. I firmly believe this will make me a better programmer and engineer.
