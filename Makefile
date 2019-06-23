XTOOLS = /opt/x-tools/mipsel-unknown-elf/bin

AS      = $(XTOOLS)/mipsel-unknown-elf-as -mips32
CC      = $(XTOOLS)/mipsel-unknown-elf-gcc
LD      = $(XTOOLS)/mipsel-unknown-elf-ld
OBJCOPY = $(XTOOLS)/mipsel-unknown-elf-objcopy

CFLAGS = -Os -DSTARTADDRESS=$(STARTADDRESS)

STARTADDRESS = 0x1000000

OBJS = start.o blink.o

.PHONY: all
all: blink.bin

blink.bin: blink.elf
	$(OBJCOPY) -O binary $< $@

blink.elf: $(OBJS) linker.lds
	$(LD) -Ttext $(STARTADDRESS) -T linker.lds -Map $(@:.elf=.map) -o $@ $+

%.o: %.[Sc]
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f *.o *.elf *.bin *~ *.map
