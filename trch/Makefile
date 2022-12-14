# Conditional Compilation for features
#CONFIGS := -DCONFIG_ENABLE_WDT_RESET
CONFIGS += -DCONFIG_FPGA_PROGRAM_MODE
CONFIGS += -DCONFIG_FPGA_WATCHDOG_TIMEOUT=3
#CONFIGS += -DCONFIG_FPGAPWR_HOLD_SEC=3
#CONFIGS += -DCONFIG_ENABLE_USART

# Design Parameter
MODULE := trch-firmware
DEVICE := 16LF877
TARGET := PPK4

# Command variables
CC     := xc8-cc -mc90lib -mcpu=$(DEVICE)
IPECMD := ipecmd.sh
RM     := rm -rf

# File Path variables
HEXDIR := hex
PRGDAT := $(HEXDIR)/$(MODULE)

# Source and object files
SRCS := src/main.c src/fpga.c src/interrupt.c src/timer.c src/usart.c src/tmp175.c src/i2c-gpio.c src/spi.c
SRCS += src/test.c
OBJS := $(SRCS:.c=.p1)

# Clean File
CF      = $(HEXDIR) src/*.p1 src/*.d MPLABXLog.* log.*

.PHONY: all
all: build

.PHONY: build
build: $(PRGDAT).hex

$(PRGDAT).hex: $(OBJS)
	mkdir -p $(HEXDIR)
	echo '*' > $(HEXDIR)/.gitignore
	$(CC) -o $(HEXDIR)/$(MODULE) $^

%.p1: %.c Makefile
	$(CC) $(CONFIGS) -c -o $@ $<

flash: program
.PHONY: program
program: $(PRGDAT).hex
	$(IPECMD) -P$(DEVICE) -T$(TARGET) -F$< -M -OL

.PHONY: erase
erase:
	$(IPECMD) -P$(DEVICE) -T$(TARGET) -E

.PHONY: reset
reset:
	$(IPECMD) -P$(DEVICE) -T$(TARGET) -OK -OL

.PHONY: halt
halt:
	$(IPECMD) -P$(DEVICE) -T$(TARGET) -OK

.PHONY: clean
clean:
	$(RM) $(CF)
