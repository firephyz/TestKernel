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

	OS_LOAD_ADDR = 0x200;
	.os_loaded 	: AT(OS_LOAD_ADDR) { build/kernel.o (.text)
										  build/kernel.o (.data)
										  build/kernel.o (.rodata)
										  build/boot.o (.idt_handlers)}

	/* Make output binary a size multiple of 512 */
	FINAL_ADDR = OS_LOAD_ADDR + SIZEOF(.os_loaded);
	ALIGN_POINT = 512 * ((FINAL_ADDR / 512) + 1) - 1;
	.end_byte : AT(ALIGN_POINT) { BYTE(0x0) }
	
	/*
	 * // Used to make the disk compatable with Bochs
	.test : AT(0x168000 - 1) { BYTE(0x5a) }
	 */


	/* Calculate the number of sectors the OS takes
	 * so we can load it later.
	 */
	OS_SECTOR_LENGTH = (. - OS_ENTRY) / 512 + 1;
}