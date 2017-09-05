PATH:=$(HOME)/programs/cross/bin:$(PATH)
DIR:=$(shell pwd)
TARGET=i686-elf
DEBUG=

all: init assemble compile link strip

init:
	mkdir -p build
	mkdir -p debug

assemble:
	$(TARGET)-as $(DEBUG) src/boot.s -o build/boot.o

compile:
	$(TARGET)-gcc $(DEBUG) -c -fno-asynchronous-unwind-tables -ffreestanding -I$(DIR)/include -o build/kernel.o src/kernel.c

link:
	$(TARGET)-ld -nostdlib -T misc/link.ld build/boot.o build/kernel.o -o build/boot.elf

strip:
	$(TARGET)-objcopy --only-keep-debug build/boot.elf debug/boot.sym
	$(shell nm $(DIR)/debug/boot.sym | awk '{ print $1" "$3}' > $(DIR)/debug/boot.sym.bochs)
	$(TARGET)-objcopy -R .comment build/boot.elf build/boot.elf
	$(TARGET)-objcopy -O binary build/boot.elf boot.bin
	$(shell cp boot.bin /home/firephyz/programs/bochs/boot.bin)


clean:
	rm -rf build
	rm -rf debug
	rm -f boot.bin
