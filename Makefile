PATH:=$(HOME)/gcc/cross/bin:$(PATH)
TARGET=i686-elf

all: init assemble compile link strip
	@echo "\nAssembled successfully."

init:
	mkdir -p build
	mkdir -p debug

assemble:
	@echo "\nAssembling..."
	$(TARGET)-as -g src/boot.s -o build/boot.o

compile:
	@echo "\nCompiling kernel..."
	$(TARGET)-gcc -c -fno-asynchronous-unwind-tables -ffreestanding src/kernel.c -o build/kernel.o

link:
	@echo "\nGenerating executable..."
	$(TARGET)-ld -nostdlib -T misc/link.ld build/boot.o build/kernel.o -o build/boot.elf

strip:
	@echo "\nGenerating debug info..."
	$(TARGET)-objcopy --only-keep-debug build/boot.elf debug/boot.sym
	$(TARGET)-objcopy -O binary build/boot.elf boot.bin




clean:
	rm -rf build
	rm -rf debug
	rm -f boot.bin
