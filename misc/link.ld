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
	.code_limit : AT(0x1B4)			{ BYTE(0x0) } 		/* Will cause a linker error if bootloader code gets too large */
	.system_id : AT(0x1B8)			{ LONG(0x7073694c) }
	.mbr	: AT(0x1BE)				{ BYTE(0x80)		/* Bootable */
									  BYTE(0x00)		/* Starting head */
									  SHORT(0x0002)		/* {[7:6],[15:8]} Starting cylinder, [5:0] Starting sector */
									  BYTE(0x0C)		/* Fat32 (LBA) */
									  BYTE(0x00)		/* Ending head */
									  SHORT(0x0083)		/* {[7:6],[15:8]} Ending cylinder, [5:0] Ending sector */
									  LONG(0x0000001)	/* Parition's starting LBA value */
									  LONG(0x00000080)	/* Total sectors in partition */
									}
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
	.os_loaded 	: AT(OS_LOAD_ADDR) { build/kernel.o (.os_entry)
									 build/boot.o (.idt_handlers)
									 build/* (.text)
									 build/* (.data)
									 build/* (.rodata)}

	FINAL_ADDR = OS_LOAD_ADDR + SIZEOF(.os_loaded);
	.end_byte : AT(66048 - 1) { BYTE(0x00) }

	/* Make output binary a size multiple of 512 */
	/*FINAL_ADDR = OS_LOAD_ADDR + SIZEOF(.os_loaded);
	ALIGN_POINT = 512 * ((FINAL_ADDR / 512) + 1) - 1;
	.end_byte : AT(ALIGN_POINT) { BYTE(0x0) }*/
	
	
	/*// Used to make the disk compatable with Bochs
	.test : AT(0x168000 - 1) { BYTE(0x5a) }
	*/


	/* Calculate the number of sectors the OS takes
	 * so we can load it later.
	 */
	OS_SECTOR_LENGTH = (. - OS_ENTRY) / 512 + 1;
}