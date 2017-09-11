PATH:=$(HOME)/programs/cross/bin:$(PATH)
DIR:=$(shell pwd)
TARGET=i686-elf
DEBUG=-g

AS=$(TARGET)-as
CC=$(TARGET)-gcc
LD=$(TARGET)-ld
OBC=$(TARGET)-objcopy

all: init assemble compile link strip
	@# Extract total size info by reading the FINAL_ADDR symbol from build/boot.elf. Hacky I know.
	@printf '\nTotal size: '
	@printf $(shell echo $$((16#$(shell readelf -a build/boot.elf | grep FINAL_ADDR  | awk -F ':' '{ print $$2 }' | awk -F ' ' '{ print $$1 }'))))
	@printf ' Bytes\n\n'

init:
	mkdir -p build
	mkdir -p debug

assemble:
	$(AS) $(DEBUG) src/boot.s -o build/boot.o

compile:
	$(CC) $(DEBUG) -O0 -c -fno-asynchronous-unwind-tables -ffreestanding -I$(DIR)/include -o build/kernel.o src/kernel.c
	$(CC) $(DEBUG) -O0 -c -fno-asynchronous-unwind-tables -ffreestanding -I$(DIR)/include -o build/console.o src/console.c
	$(CC) $(DEBUG) -O0 -c -fno-asynchronous-unwind-tables -ffreestanding -I$(DIR)/include -o build/keyboard.o src/keyboard.c
	$(CC) $(DEBUG) -O0 -c -fno-asynchronous-unwind-tables -ffreestanding -I$(DIR)/include -o build/pci_ide.o src/pci_ide.c

link:
	$(LD) -nostdlib -T misc/link.ld build/boot.o build/kernel.o build/pci_ide.o build/console.o build/keyboard.o -o build/boot.elf

strip:
	$(OBC) --only-keep-debug build/boot.elf debug/boot.sym
	$(OBC) -R .comment build/boot.elf build/boot.elf
	$(OBC) -O binary build/boot.elf boot.bin


clean:
	rm -rf build
	rm -rf debug
	rm -f boot.bin
