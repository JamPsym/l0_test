# Define paths
CUBEL0_DIR = STM32CubeL0
TOOLCHAIN = arm-none-eabi-
CC = $(TOOLCHAIN)gcc
LD = $(TOOLCHAIN)ld
OBJCOPY = $(TOOLCHAIN)objcopy
SIZE = $(TOOLCHAIN)size

MCU = -mcpu=cortex-m0plus -mthumb
DEFS = -DSTM32L053xx  

# Include paths (CMSIS focus; add HAL if needed later)
INCLUDES = \
	-Iinc \
	-I$(CUBEL0_DIR)/Drivers/CMSIS/Device/ST/STM32L0xx/Include \
	-I$(CUBEL0_DIR)/Drivers/CMSIS/Include

# Sources: main.c + CMSIS system/startup files
CSRCS = \
	main.c \
	system_stm32l0xx.c 

ASRCS = startup_stm32l053xx.s 

# Linker script
LDSCRIPT = STM32L053C8Tx_FLASH.ld  # In project root

CFLAGS = $(MCU) -g -O0 -Wall $(DEFS) $(INCLUDES)
LDFLAGS = $(MCU) -T$(LDSCRIPT) -Wl,--gc-sections

COBJS = $(CSRCS:.c=.o)
AOBJS = $(ASRCS:.s=.o)


all: app.elf

app.elf: $(COBJS) $(AOBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	$(OBJCOPY) -O binary $@ app.bin
	$(SIZE) $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(COBJS) $(AOBJS) app.elf app.bin

# Optional: Init submodules if needed
submodules:
	git submodule update --init --recursive

.PHONY: all clean submodules
