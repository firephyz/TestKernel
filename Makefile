PATH:=$(HOME)/programs/cross/bin:$(PATH)
TARGET=i686-elf

all: init assemble compile link strip

init:
	mkdir -p build
	mkdir -p debug

assemble:
	$(TARGET)-as -g src/boot.s -o build/boot.o

compile:
	$(TARGET)-gcc -g -c -fno-asynchronous-unwind-tables -ffreestanding src/kernel.c -o build/kernel.o

link:
	$(TARGET)-ld -nostdlib -T misc/link.ld build/boot.o build/kernel.o -o build/boot.elf

strip:
	$(TARGET)-objcopy --only-keep-debug build/boot.elf debug/boot.sym
	$(TARGET)-objcopy -O binary build/boot.elf boot.bin


clean:
	rm -rf build
	rm -rf debug
	rm -f boot.bin
