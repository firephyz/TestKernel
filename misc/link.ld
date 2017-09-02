/*
 * Must set the address to 0x7C00 so that the debugger knows
 * where the code is when loaded by BIOS.
 * All of this is on a "disk". The bootloader must load
 * the data structures and OS into memory and then go into
 * protected mode.
 */

LOAD_ADDRESS = 0x7C00;
OS_ENTRY = 0x100000;

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

	/*
	 * '.' sets the VMA. That is the address in the output binary.
	 * The AT function then sets the LMA. This is the address the section
	 * will have when it is run. This allows us to debug the kernel
	 * once it is relocated to OS_ENTRY.
	 */
	. = OS_ENTRY;
	.os_text : AT ( 0x7E00 ) { build/kernel.o (.text) }
	.os_data : AT ( 0x7E00 + SIZEOF (.os_text) ) { build/kernel.o (.data) }

	/* Calculate the number of sectors the OS takes
	 * so we can load it later.
	 */
	OS_SECTOR_LENGTH = (. - OS_ENTRY) / 512 + 1;
}