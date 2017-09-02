/*
 * Must set the address to 0x7C00 so that the debugger knows
 * where the code is when loaded by BIOS.
 * All of this is on a "disk". The bootloader must load
 * the data structures and OS into memory and then go into
 * protected mode.
 */

LOAD_ADDRESS = 0x7C00;

SECTIONS
{
	/**************
	 * Bootloader *
	 **************/

	. = LOAD_ADDRESS;
	.text : { build/boot.o (.text) }
	.data : { build/boot.o (.data) }

	. = LOAD_ADDRESS + 0x1FE;
	.magic : { SHORT(0xAA55) }

	/***************
	 * Kernel code *
	 ***************/

	. = 0x7E00;
	.os_text : { build/kernel.o (.text) }
	.os_data : { build/kernel.o (.data) }
}