ENTRY(_boot_start)

SECTIONS {

/*********************
 * Bootloader Part 1 *
 *********************/
	.boot BOOT_LOAD_ADDR 		: AT(0) 			{ build/boot.o (.text)
									  				  build/boot.o (.data) }

/**********************
 * Master Boot Record *
 **********************/
	.code_limit 				: AT(0x1B4)			{ BYTE(0x0) } 		/* Will cause a linker error if bootloader code gets too large */
	.system_id 					: AT(0x1B8)			{ LONG(0x7073694c) }
	.mbr						: AT(0x1BE)			{ BYTE(0x80)		/* Bootable */
													  BYTE(0x00)		/* Starting head */
													  SHORT(0x0001)		/* {[7:6],[15:8]} Starting cylinder, [5:0] Starting sector */
													  BYTE(0x83)		/* Fat32 (LBA) */
													  BYTE(0x00)		/* Ending head */
													  SHORT(0x0202)		/* {[7:6],[15:8]} Ending cylinder, [5:0] Ending sector */
													  LONG(0x00000000)	/* Parition's starting LBA value */
													  LONG(0x00000080) }/* Total sectors in partition */
	.magic 						: AT(0x1FE) 		{ SHORT(0xAA55) }

/*********************
 * Bootloader Part 2 *
 *********************/
	.boot_cont BOOT_CONTINUED 	: AT(0x200)		{ build/boot.o (.boot_cont) }

/***************
 * Kernel code *
 ***************/
	.os_start OS_ENTRY 			: AT(0x400)		{ build/kernel.o (.os_entry)
												  build/* (.text) 
												  build/* (.data) 
												  build/* (.rodata*) }

	OS_SECTOR_COUNT = (. - OS_ENTRY) / 512 + 1;
	TOTAL_SIZE = 0x400 + SIZEOF(.os_start);

	.end_byte					: AT(0xFFFF)	{ BYTE(0x00) }
}