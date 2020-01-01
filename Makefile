CROSS_COMPILE = /opt/x-tools/mipsel-unknown-elf/bin/mipsel-unknown-elf-

AS      = $(CROSS_COMPILE)as -mips32
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -Os -DSTARTADDRESS=$(STARTADDRESS) -Wall -Wextra

STARTADDRESS = 0x1000000

TARGETS = blink.bin

.PHONY: all
all: $(TARGETS)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.elf: start.o %.o
	$(LD) -Ttext $(STARTADDRESS) -T linker.lds -Map $(@:.elf=.map) -o $@ $+

%.o: %.[Sc]
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f *.o *.elf *.bin *~ *.map

# ELF images can be used in radare2
.PRECIOUS: %.elf
	
