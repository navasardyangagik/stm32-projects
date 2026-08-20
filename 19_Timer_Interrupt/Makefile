TOOLCHAIN = /opt/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.14.3.rel1.linux64_1.0.100.202602081740/tools/bin
PROGRAMMER = /opt/stm32cubeide/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.linux64_2.2.400.202601091506/tools/bin/STM32_Programmer_CLI

CC = $(TOOLCHAIN)/arm-none-eabi-gcc

CMSIS = ../chip_headers/CMSIS
INCLUDES = -I$(CMSIS)/Include -I$(CMSIS)/Device/ST/STM32F4xx/Include -IInc

CFLAGS = -mcpu=cortex-m4 -mthumb -std=c11 -O0 -g -Wall -DSTM32F411xE $(INCLUDES)
LDFLAGS = -T STM32F411RETX_FLASH.ld -Wl,--gc-sections --specs=nosys.specs

SRCS = Src/main.c Src/syscalls.c Src/sysmem.c Startup/startup_stm32f411retx.s
TARGET = firmware.elf

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) -o $(TARGET)

flash: $(TARGET)
	$(PROGRAMMER) -c port=SWD -w $(TARGET) -rst

clean:
	rm -f $(TARGET)

.PHONY: all flash clean
