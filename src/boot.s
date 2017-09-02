.set GDT_DESC_LIMIT, 4
.set CODE_SELECTOR, 0x08
.set DATA_SELECTOR, 0x10

.global OS_SECTOR_LENGTH
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

	lgdtl gdt_desc

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

	mov $OS_SECTOR_LENGTH, %di
	mov $0x0002, %cx // Start reading from the second sector
	mov $0x7E00, %bx // Buffer is at 0x7E00
	mov $0x0080, %dx // Head 0 and drive 80 (hard drive)

load_os_loop:
	// Fetch a sector from the disk
	mov $0x0201, %ax // Read and get 1 sector
	int $0x13
	sub $1, %di
	jnz load_os_loop

	pop %di
	pop %ebx
	ret

_protected_mode:
	.code32

	// Init the segment registers
	movw $DATA_SELECTOR, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	calll OS_ENTRY

	hlt
	jmp -2

gdt_desc:
	.word GDT_DESC_LIMIT * 8 // One descriptor is 8 bytes long
	.long gdt

gdt:
	// Null selector
	.word	0, 0
	.byte	0, 0, 0, 0

	// Code selector
	.word	0xFFFF, 0
	.byte	0, 0x9A, 0xCF, 0

	// Data selector
	.word	0xFFFF, 0
	.byte	0, 0x92, 0xCF, 0
