PATH:=$(HOME)/programs/cross/bin:$(PATH)
DIR:=$(shell pwd)
TARGET=i686-elf
DEBUG=-g

all: init assemble compile link strip
	@# Extract total size info by reading the FINAL_ADDR symbol from build/boot.elf. Hacky I know.
	@printf '\nTotal size: '
	@printf $(shell echo $$((16#$(shell readelf -a build/boot.elf | grep FINAL_ADDR  | awk -F ':' '{ print $$2 }' | awk -F ' ' '{ print $$1 }'))))
	@printf ' Bytes\n\n'

init:
	mkdir -p build
	mkdir -p debug

assemble:
	$(TARGET)-as $(DEBUG) src/boot.s -o build/boot.o

compile:
	$(TARGET)-gcc $(DEBUG) -O0 -c -fno-asynchronous-unwind-tables -ffreestanding -I$(DIR)/include -o build/kernel.o src/kernel.c
	$(TARGET)-gcc $(DEBUG) -O0 -c -fno-asynchronous-unwind-tables -ffreestanding -I$(DIR)/include -o build/console.o src/console.c
	$(TARGET)-gcc $(DEBUG) -O0 -c -fno-asynchronous-unwind-tables -ffreestanding -I$(DIR)/include -o build/keyboard.o src/keyboard.c
	$(TARGET)-gcc $(DEBUG) -O0 -c -fno-asynchronous-unwind-tables -ffreestanding -I$(DIR)/include -o build/pci_ide.o src/pci_ide.c

link:
	$(TARGET)-ld -nostdlib -T misc/link.ld build/boot.o build/kernel.o build/pci_ide.o build/console.o build/keyboard.o -o build/boot.elf

strip:
	$(TARGET)-objcopy --only-keep-debug build/boot.elf debug/boot.sym
	$(TARGET)-objcopy -R .comment build/boot.elf build/boot.elf
	$(TARGET)-objcopy -O binary build/boot.elf boot.bin


clean:
	rm -rf build
	rm -rf debug
	rm -f boot.bin
