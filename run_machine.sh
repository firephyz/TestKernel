#!/bin/bash

DEBUG=

if [ "$1" = "-debug" ]; then
	DEBUG="-s -S"
fi

if [ ! -z "$DEBUG" ]; then
	konsole -e bash -c 'cd ~/dev/kernel && gdb -iex "source misc/debug.gdbinit"'
fi

# Qemu's disk geometry detection is very strange. Will manually specify for now. Also, the cylinders
# argument must be (# of cylinders) + 2. Don't know why but Seabios thinks it
# actually has 2 less cylinders than what you specify here. So for my 129 sector disk right now,
# chs (2, 0, 3) is the last sector. (63 - 1) + 63 + 3.
# Option to add if we want seabios to output debug info.
# -chardev stdio,id=seabios -device isa-debugcon,iobase=0x402,chardev=seabios
qemu-system-i386 $DEBUG																\
				 -enable-kvm                          								\
                 -drive file=./boot.bin,if=ide,format=raw,cyls=5,heads=1,secs=63	\
                 -vga std                             								\
                 -m 1024M                             								\
                 -boot d                              								\
                 -name "Lisp VM"                      								\
                 -bios /home/firephyz/src/seabios/out/bios.bin 						\
                 -monitor stdio