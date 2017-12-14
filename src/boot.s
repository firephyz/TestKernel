.code16
.global BOOT_LOAD_ADDR
.set BOOT_LOAD_ADDR,	0x7C00

.set STACK_TOP, 		BOOT_LOAD_ADDR
.set BOOT_DRIVE, 		STACK_TOP - 2
.set DRIVE_PARAMS,		BOOT_DRIVE - 4 // 4 Bytes. First two hold [heads, sectors], last two hold [cylinders]

.text
.global _boot_start
_boot_start:
	cli

	// Make sure the data register and stack are set correctly
	xor %ax, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss

	lea STACK_TOP, %sp
	push %dx // Save boot drive number
	sub $0x4, %sp // Reserve space for the drive_params structure

	call _get_drive_params

	call _load_boot_loader

	call _enable_a20

	call _load_os

	// Haven't set up the IDT yet so having hardware interrupts
	// would cause triple faults. Not good.
	call _disable_hardware_irqs

	// Enter protected mode
	lgdtl _gdt_desc
	movl %cr0, %eax
	orl $0x01, %eax
	movl %eax, %cr0

	// Execute far jump to load new segment registers from gdt
	ljmpl $0x08, $_os_boot

_get_drive_params:
	// Get drive parameters
	mov BOOT_DRIVE, %dl
	xor %di, %di
	mov $0x08, %ah
	int $0x13
	jc _get_drive_params_failed
	xor %ax, %ax
	mov %ax, %es

	// Store the params on the stack for later use
	mov %dh, DRIVE_PARAMS // dh has max head number
	mov %ch, %dl
	mov %cl, %dh
	shr $6, %dh // dx holds max cylinder number
	mov %dx, DRIVE_PARAMS + 2
	and $0x3F, %cl // Extract max sector number
	mov %cl, DRIVE_PARAMS + 1

	ret

.set BOOT_CONTINUED,		0x7E00 // Address for the remaining bootloader code
.global BOOT_CONTINUED
_load_boot_loader:
	mov %sp, %bp

	// Read a sector from memory
	mov $0x0201, %ax
	mov $2, %cl // Grab second sector of boot loader
	mov $0x00, %ch
	xor %dx, %dx
	mov BOOT_DRIVE, %dl
	lea BOOT_CONTINUED, %bx
	int $0x13
	jc _get_drive_params_failed

	ret

_check_a20:
	push %ds
	// Attempt to fetch MBR magic number at 0x7DFE but do so a megabyte later.
	// If a20 isn't enabled, it will wrap making address 0x7DFE will be the same as address 0x107DFE
	// Otherwise, the two address will refer to different physical locations
	mov $0xFFFF, %ax
	mov %ax, %ds
	mov 0x7E0E, %ax
	cmp $0xAA55, %ax
	jne _check_a20_yes_done // They aren't equal so address space doesn't wrap. A20 is enabled

	// The two are equal. Likely wrapping but it could be a coincidence.
	// Change the magic number and try again.
	xor %ax, %ax
	mov %ax, %ds
	movw $0x1234, 0x7DFE
	// Fetch that value again
	mov $0xFFFF, %ax
	mov %ax, %ds
	mov 0x7E0E, %ax
	cmp $0x1234, %ax
	jne _check_a20_yes_done // They actually aren't equal afterall. A20 is enabled

	// They are equal again, the A20 isn't enabled
	mov $0, %ax
	jmp _check_a20_ret

_check_a20_yes_done:
	mov $1, %ax

_check_a20_ret:
	xor %dx, %dx // Restore the original magic number
	mov %dx, %ds
	movw $0xAA55, 0x7DFE
	pop %ds
	ret
	
_enable_a20:
	call _check_a20
	cmp $1, %ax
	je _enable_a20_ret

	inb	$0x92
	and $(~0x03), %al
	or	$0x02, %al
	outb $0x92

	call _check_a20
	cmp $1, %ax
	jne _enable_a20_failed

_enable_a20_ret:
	ret

_disable_hardware_irqs:
	mov $0xFF, %al
	out %al, $0x21
	out %al, $0xA1
	ret

.code32 // Jump here once we enter protected mode
_os_boot:
	lea STACK_TOP, %esp // Don't need the old stack anymore

	// Init segment registers for 32 bit mode
	movw $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	// Call main OS
	call OS_ENTRY

	hlt
	jmp -2
.code16

/*********************
 * Failure functions *
 *********************/

_load_os_failed:
	lea _load_os_failed_string, %ax
	mov $0xC7, %bl
	mov $18, %cx
	mov $0x0000, %dx
	call _print_string
	jmp _boot_failed

_load_boot_loader_failed:
	lea _load_boot_loader_failed_string, %ax
	mov $0xC7, %bl
	mov $33, %cx
	mov $0x0000, %dx
	call _print_string
	jmp _boot_failed

_get_drive_params_failed:
	lea _get_drive_params_failed_string, %ax
	mov $0xC7, %bl
	mov $20, %cx
	mov $0x0000, %dx
	call _print_string
	jmp _boot_failed

_enable_a20_failed:
	hlt
	jmp -2
	lea _a20_failed_string, %ax
	mov $0xC7, %bl
	mov $11, %cx
	mov $0x0000, %dx
	call _print_string
	jmp _boot_failed

_boot_failed:
	hlt
	jmp -2

	// @ax address of string
	// @bl color attribute
	// @es segment for string
	// @cx length of string
	// @dx row, column
_print_string:
	push %bx
	mov %ax, %bp
	mov $0x00, %bh
	mov $0x1301, %ax
	int $0x10
	pop %bx
	ret

.data
_a20_failed_string:
	.ascii "A20 failed."
_get_drive_params_failed_string:
	.ascii "Drive params failed."
_load_os_failed_string:
	.ascii "OS loading failed."
_load_boot_loader_failed_string:
	.ascii "Bootloader-2 loading failed."

/*****************************
 * Part 2 of the boot loader *
 *****************************/

.section .boot_cont, "ax"

.global OS_ENTRY

.set OS_ENTRY, 0x100000
.set OS_LOAD_BUF, 	0x8000
.set OS_ENTRY_OFFSET, OS_ENTRY - 0xFFFF0 // Offset for OS_ENTRY with 0xFFFF segment
.set OS_START_SECTOR, 3 // Start with 3rd sector on disk
_load_os:
	mov %sp, %bp
	mov $OS_START_SECTOR, %ax
	push %ax // Space for current sector load index

_load_os_loop:
	// Read a sector from memory
	mov $0x0201, %ax
	mov -2(%bp), %cx
	mov $0x00, %ch
	xor %dx, %dx
	mov BOOT_DRIVE, %dl
	lea OS_LOAD_BUF, %bx
	int $0x13

	jc _load_os_failed

	mov $0xFFFF, %ax // Init es register for repmov instruction
	mov %ax, %es
	lea OS_LOAD_BUF, %si
	mov -2(%bp), %bx
	sub $OS_START_SECTOR, %bx
	imul $512, %bx
	lea OS_ENTRY_OFFSET(%bx), %di
	mov $128, %cx // 512 bytes in a sector. 128 dword moves
	rep
	movsl

	xor %ax, %ax // Reset es register
	mov %ax, %es

	// Check and see if we have more sectors to read
	incw -2(%bp)
	mov -2(%bp), %dx
	sub $OS_START_SECTOR, %dx
	cmp $OS_SECTOR_COUNT, %dx
	jne _load_os_loop

	pop %ax
	ret

_print_registers:
	push %sp
	push %bp
	mov %sp, %bp
	add $4, %bp
	push %ax
	push %bx
	push %cx
	push %dx
	push %si
	push %di

	mov -6(%bp), %dx
	mov $0x0002, %cx
	call _draw_number

	mov -8(%bp), %dx
	mov $0x0003, %cx
	call _draw_number

	mov -10(%bp), %dx
	mov $0x0004, %cx
	call _draw_number

	mov -12(%bp), %dx
	mov $0x0005, %cx
	call _draw_number

	mov -14(%bp), %dx
	mov $0x0006, %cx
	call _draw_number

	mov -16(%bp), %dx
	mov $0x0007, %cx
	call _draw_number

	mov -2(%bp), %dx
	mov $0x0008, %cx
	call _draw_number

	mov -4(%bp), %dx
	mov $0x0009, %cx
	call _draw_number

	add $12, %sp
	pop %bp
	pop %sp
	ret

	// @dx number
	// @cx column, row
_draw_number:
	push %bp

	mov %sp, %bp
	push %cx
	push %dx
	xor %si, %si
	push %si

	mov -4(%bp), %ax
	cmp $0, %ax
	je _draw_number_done

_draw_number_loop:
	and $0x8000, %ax
	jz _draw_number_next

	mov -2(%bp), %cx
	mov -6(%bp), %dx
	shl $8, %dx
	add %dx, %cx
	call _draw_square

_draw_number_next:
	incw -6(%bp)
	mov -4(%bp), %ax
	shl $1, %ax
	mov %ax, -4(%bp)
	cmp $0, %ax
	jne _draw_number_loop

_draw_number_done:
	add $6, %sp
	pop %bp
	ret

	// @cx column, row
_draw_square:
	push %bp
	push %ds

	mov $0xb000, %ax
	mov %ax, %ds
	movzx %ch, %di
	shl $1, %di
	and $0x00, %ch
	imul $160, %cx, %bx
	movw $0x1720, 0x8000(%bx, %di, 1)

	pop %ds
	pop %bp
	ret

/***************************
 * Global Descriptor Table *
 ***************************/
.code32
.set GDT_DESC_LIMIT, 4

_gdt_desc:
	.word GDT_DESC_LIMIT * 8 - 1// One descriptor is 8 bytes long
	.long _gdt

.align 8
_gdt:
	// Null selector
	.word 0x0000, 0x0000
	.byte 0x00, 0x00, 0x00, 0x00

	// Code selector
	.word 0xFFFF, 0
	.byte 0, 0x9A, 0xCF, 0

	// Data selector
	.word 0xFFFF, 0
	.byte 0, 0x92, 0xCF, 0

	// Null selector
	.word 0, 0
	.byte 0, 0, 0, 0

/**********************
 * Interrupt Handlers *
 **********************/

.section .idt_handlers, "ax"

.global _interrupt_00
_interrupt_00:
	call handle_int_00
	iret

.global _interrupt_09
_interrupt_09:
	call handle_int_09
	iret

.global _interrupt_xx
_interrupt_xx:
	call handle_int_xx
	iret
