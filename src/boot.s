.set GDT_DESC_LIMIT, 4
.set CODE_SELECTOR, 0x08
.set DATA_SELECTOR, 0x10
.set OS_TEMP_BUFFER, 0x7E00
.set MAX_SECTOR_LOAD, 128
.set LOAD_ADDRESS, 0x7C00
.set OS_ENTRY, 0x100000

// Make these variables available to C files
.global LOAD_ADDRESS
.global OS_ENTRY
.global CODE_SELECTOR

.code16

.text
_start:
	cli

	// Make sure the data register and stack are set correctly
	xor %eax, %eax
	mov %ax, %ds
	mov %ax, %es
	mov $0x7C00, %esp

	// Enable the A20 line
	call _enable_A20

	// Load the OS into memory
	call _load_os

	lgdtl _gdt_desc

	// Set the PE flag bit and enter into 32 bit protected mode!
	movl %cr0, %eax
	orl $0x01, %eax
	movl %eax, %cr0

	// Must far jump to fill CS with the new GDT data
	ljmpl $CODE_SELECTOR, $_protected_mode

_enable_A20:
	ret

_load_os:
	push %ebx
	push %di

	// Reset the disk
	mov $0x00, %ah
	mov $0x80, %dl
	int $0x13

	mov $0, %di
	mov $0x0002, %cx // Start reading from the second sector
	mov $OS_TEMP_BUFFER, %bx // Buffer is at 0x7E00
	mov $0x0080, %dx // Head 0 and drive 80 (hard drive)

_load_os_loop:
	// Fetch a sector from the disk
	mov $0x0201, %ax // Read and get 1 sector
	int $0x13
	add $1, %di
	add $1, %cx
	add $512, %bx
	cmp $MAX_SECTOR_LOAD, %di
	je _os_too_large
	cmp $OS_SECTOR_LENGTH, %di
	jne _load_os_loop

	pop %di
	pop %ebx
	ret

_os_too_large:
	hlt
	jmp -2

/******************************
 * 32 bit protected mode code *
 ******************************/

.macro OUTBYTE port, value
	mov \value, %al
	out %al, \port
.endm

.code32
_protected_mode:

	// Init the segment registers
	movw $DATA_SELECTOR, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	// Set the source and dest registers. These will automatically
	// increment and be ready for the next loop if need be
	mov $OS_ENTRY, %edi
	mov $OS_TEMP_BUFFER, %esi
	mov $0, %dx

_move_os_loop:
	// Move OS code to where it needs to go
	mov $128, %cx
	rep
	movsd
	add $1, %dx
	cmp $OS_SECTOR_LENGTH, %dx
	jne _move_os_loop

	// Init the PIC
	OUTBYTE $0x20, $0x11
	OUTBYTE $0xA0, $0x11

	OUTBYTE $0x21, $0x20
	OUTBYTE $0xA1, $0x28

	OUTBYTE $0x21, $0x04
	OUTBYTE $0xA1, $0x02

	OUTBYTE $0x21, $0x05
	OUTBYTE $0xA1, $0x01

	// Only enable IRQ 0
	OUTBYTE $0x21, $0xFD
	OUTBYTE $0xA1, $0xFF

	xor %eax, %eax
	mov %eax, %ebx
	mov %eax, %ecx
	mov %eax, %edx
	mov %eax, %esi
	mov %eax, %edi
	call OS_ENTRY

	hlt
	jmp -2

/***************************
 * Global Descriptor Table *
 ***************************/

_gdt_desc:
	.word GDT_DESC_LIMIT * 8 // One descriptor is 8 bytes long
	.long _gdt

.align 8
_gdt:
	// Null selector
	.word	0, 0
	.byte	0, 0, 0, 0

	// Code selector
	.word	0xFFFF, 0
	.byte	0, 0x9A, 0xCF, 0

	// Data selector
	.word	0xFFFF, 0
	.byte	0, 0x92, 0xCF, 0

/**********************
 * Interrupt Handlers *
 **********************/

.section .idt_handlers, "ax"

.global _interrupt_09
_interrupt_09:
	call handle_int_09
	iret

.global _interrupt_xx
_interrupt_xx:
	call handle_int_xx
	iret
