CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m4 -mthumb -std=c11 -O0 -g -Wall
LDFLAGS = -T STM32F411RETX_FLASH.ld -Wl,--gc-sections --specs=nosys.specs

SRCS = Src/main.c Src/syscalls.c Src/sysmem.c Startup/startup_stm32f411retx.s
TARGET = firmware.elf

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) -o $(TARGET)

flash: $(TARGET)
	sudo openocd -f interface/stlink.cfg \
	 			 -f target/stm32f4x.cfg \
				 -c "program $(TARGET) verify reset exit"
				 
clean:
	rm -f $(TARGET)