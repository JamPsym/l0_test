#
# Makefile for STM32L053xx project
#

#
# Toolchain
#
TOOLCHAIN = arm-none-eabi-
CC = $(TOOLCHAIN)gcc
LD = $(TOOLCHAIN)ld
OBJCOPY = $(TOOLCHAIN)objcopy
SIZE = $(TOOLCHAIN)size

#
# MCU flags
#
MCU = -mcpu=cortex-m0plus -mthumb

#
# Project settings
#
PROJECT = app
LDSCRIPT = STM32L053C8Tx_FLASH.ld

#
# Directories
#
BUILD_DIR = build
CUBEL0_DIR = ../STM32CubeL0
LL_SRC_DIR = $(CUBEL0_DIR)/Drivers/STM32L0xx_HAL_Driver/Src

#
# Source files
#
CSRCS = \
	main.c \
	system_stm32l0xx.c

ASRCS = startup_stm32l053xx.s

LLSRCS_NAMES = $(notdir $(filter-out %stm32l0xx_ll_usb.c, $(wildcard $(LL_SRC_DIR)/stm32l0xx_ll_*.c)))

#
# Object files
#
COBJS = $(addprefix $(BUILD_DIR)/, $(CSRCS:.c=.o))
AOBJS = $(addprefix $(BUILD_DIR)/, $(ASRCS:.s=.o))
LLOBJS = $(addprefix $(BUILD_DIR)/, $(LLSRCS_NAMES:.c=.o))

OBJS = $(COBJS) $(AOBJS) $(LLOBJS)

#
# Include paths
#
INCLUDES = \
	-Iinc \
	-I$(CUBEL0_DIR)/Drivers/CMSIS/Device/ST/STM32L0xx/Include \
	-I$(CUBEL0_DIR)/Drivers/CMSIS/Include \
	-I$(CUBEL0_DIR)/Drivers/STM32L0xx_HAL_Driver/Inc

#
# Compiler and linker flags
#
DEFS = -DSTM32L053xx -DUSE_FULL_LL_DRIVER
CFLAGS = $(MCU) -g -O0 -Wall $(DEFS) $(INCLUDES)
LDFLAGS = $(MCU) -T$(LDSCRIPT) -Wl,--gc-sections

#
# Targets
#
all: $(PROJECT).elf

$(PROJECT).elf: $(OBJS)
	@echo "Linking $@"
	@$(CC) $(LDFLAGS) $^ -o $@
	@echo "Creating binary..."
	@$(OBJCOPY) -O binary $@ $(PROJECT).bin
	@echo "Size of $(PROJECT).elf:"
	@$(SIZE) $@

$(BUILD_DIR)/%.o: $(LL_SRC_DIR)/%.c
	@mkdir -p $(@D)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(@D)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR) $(PROJECT).elf $(PROJECT).bin

#
# Phony targets
#
.PHONY: all clean submodules flash gdb

submodules:
	git submodule update --init --recursive

flash: $(PROJECT).elf
	gdb-multiarch -ex 'target extended-remote localhost:4242' -ex 'load' -ex 'monitor reset halt' -ex 'monitor flash write_image erase' -ex 'monitor reset' -ex 'quit' $(PROJECT).elf

gdb: $(PROJECT).elf
	gdb-multiarch -ex 'target extended-remote localhost:4242' -ex 'load' -ex 'break main' -ex 'svd_load STM32L053.svd' -ex 'continue' $(PROJECT).elf
