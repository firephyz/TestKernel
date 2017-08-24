PATH:=$(HOME)/programs/cross/bin:$(PATH)
TARGET=i686-elf

all: init assemble link strip
	@echo -e "\nAssembled successfully."

init:
	mkdir -p build
	mkdir -p debug

assemble:
	echo "Assembling..."
	$(TARGET)-as -g src/boot.s -o build/boot.o

link:
	@echo -e "\nGenerating executable..."
	$(TARGET)-ld -T misc/link.ld build/boot.o -o build/boot.elf

strip:
	@echo -e "\nGenerating debug info..."
	$(TARGET)-objcopy --only-keep-debug build/boot.elf debug/boot.sym
	$(TARGET)-objcopy -O binary build/boot.elf boot.bin




clean:
	rm -rf build
	rm -rf debug
	rm -f boot.bin