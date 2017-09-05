/*
 * Must set the address to 0x7C00 so that the debugger knows
 * where the code is when loaded by BIOS.
 * All of this is on a "disk". The bootloader must load
 * the data structures and OS into memory and then go into
 * protected mode.
 */

SECTIONS
{
	/**************
	 * Bootloader *
	 **************/

	. = LOAD_ADDRESS;
	.text 	: AT(0) 				{ build/boot.o (.text) }
	.data 	: AT(SIZEOF(.text)) 	{ build/boot.o (.data) }
	.magic 	: AT(0x1FE) 			{ SHORT(0xAA55) }

	/***************
	 * Kernel code *
	 ***************/

	/*
	 * The AT function sets the LMA. That is the address in the output binary.
	 * Then '.' sets the VMA. This is the address the section
	 * will have when it is run. This allows us to debug the kernel
	 * once it is relocated to OS_ENTRY.
	 */
	. = OS_ENTRY;

	OS_TEXT_LOAD_ADDR = 0x200;
	.os_text 	: AT(OS_TEXT_LOAD_ADDR) 	{ build/kernel.o (.text) }

	OS_DATA_LOAD_ADDR = OS_TEXT_LOAD_ADDR + SIZEOF(.os_text);
	.os_data 	: AT(OS_DATA_LOAD_ADDR) 	{ build/kernel.o (.data) }

	OS_RODATA_LOAD_ADDR = OS_DATA_LOAD_ADDR + SIZEOF(.os_data);
	.os_rodata 	: AT(OS_RODATA_LOAD_ADDR) 	{ build/kernel.o (.rodata) }


	/**********************
	 * Interrupt Handlers *
	 **********************/

	 IDT_LOAD_ADDR = OS_RODATA_LOAD_ADDR + SIZEOF(.os_rodata);
	.idt_handlers 	: AT(IDT_LOAD_ADDR)	{ build/boot.o (.idt_handlers) }


	.test : AT(0x167FFF) { BYTE(0xa5) }


	/* Calculate the number of sectors the OS takes
	 * so we can load it later.
	 */
	OS_SECTOR_LENGTH = (. - OS_ENTRY) / 512 + 1;
}