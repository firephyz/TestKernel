#!/bin/bash

DEBUG=

if [ "$1" = "-debug" ]; then
	DEBUG="-s -S"
fi

if [ ! -z "$DEBUG" ]; then
	konsole -e bash -c 'cd ~/dev/kernel && gdb -iex "source misc/debug.gdbinit"'
fi

qemu-system-i386 $DEBUG										\
				 -enable-kvm                          		\
                 -drive file=./boot.bin,if=ide,format=raw	\
                 -vga std                             		\
                 -m 1024M                             		\
                 -boot d                              		\
                 -name "Lisp VM"                      		\
                 -monitor
