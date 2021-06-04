CROSS_COMPILE = /opt/x-tools/mipsel-unknown-elf/bin/mipsel-unknown-elf-

AS      = $(CROSS_COMPILE)as -mips32
CC      = $(CROSS_COMPILE)gcc
CXX     = $(CROSS_COMPILE)g++
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS   = -Os -DSTARTADDRESS=$(STARTADDRESS) -Wall -Wextra -nostdlib
CXXFLAGS = -std=c++17 -fno-rtti -fno-unwind-tables -fno-exceptions

STARTADDRESS = 0x1000000

TARGETS = blink.bin

.PHONY: all
all: $(TARGETS)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

# FIXME: order of objects should not matter
%.elf: start.o %.o
	$(LD) -Ttext $(STARTADDRESS) -T linker.lds -Map $(@:.elf=.map) -o $@ $+

%.o: %.[Sc]
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f *.o *.elf *.bin *~ *.map

# ELF images can be used in cutter/radare2
.PRECIOUS: %.elf

