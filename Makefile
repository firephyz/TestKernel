SHELL=/bin/bash
PATH:=$(HOME)/programs/cross/bin:$(PATH)
DIR=$(shell pwd)
TARGET=i686-elf

AS=$(TARGET)-as
CC=$(TARGET)-gcc
LD=$(TARGET)-ld
OBC=$(TARGET)-objcopy

SOURCES=$(wildcard src/*.c)
FILES=$(patsubst src/%.c,%,$(SOURCES))
OBJS=$(patsubst %,build/%.o,$(FILES))

DEBUG=-g
OPTIMIZE=-O0
COMPILE_FLAGS=-mno-80387 -fno-asynchronous-unwind-tables -ffreestanding -I$(DIR)/include

all: strip
	@# Extract total size info by reading the FINAL_ADDR symbol from build/boot.elf. Hacky I know.
	@printf '\nTotal size: '
	@printf $(shell echo $$((16#$(shell readelf -a build/boot.elf | grep TOTAL_SIZE | awk -F ':' '{ print $$2 }' | awk -F ' ' '{ print $$1 }'))))
	@printf ' Bytes\n\n'

init:
	mkdir -p build
	mkdir -p debug

assemble: | init
	$(AS) $(DEBUG) src/boot.s -o build/boot.o

build/%.o : src/%.c
	$(CC) -c $(DEBUG) $(OPTIMIZE) $(COMPILE_FLAGS) -o $@ $(patsubst build%.o,src%.c,$@)

link: assemble $(OBJS)
	$(LD) -nostdlib -T misc/link.ld $(OBJS) build/boot.o -o build/boot.elf

strip: link
	$(OBC) --only-keep-debug build/boot.elf debug/boot.sym
	$(OBC) -R .comment build/boot.elf build/boot.elf
	$(OBC) -O binary build/boot.elf boot.bin

clean:
	rm -rf build
	rm -rf debug
	rm -f boot.bin
