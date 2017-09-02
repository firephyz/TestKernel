.set GDT_DESC_LIMIT, 4
.set CODE_SELECTOR, 0x08
.set DATA_SELECTOR, 0x10

.code16

.text
_start:
	cli

	// Make sure the data and code registers are set correctly
	xor %eax, %eax
	mov %ax, %ds

	lgdtl gdt_desc

	// Set the PE flag bit and enter into 32 bit protected mode!
	movl %cr0, %eax
	orl $0x01, %eax
	movl %eax, %cr0

	// Must far jump to fill CS with the new GDT data
	ljmpl $CODE_SELECTOR, $_protected_mode

_protected_mode:
	.code32

	// Init the segment registers
	movw $DATA_SELECTOR, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	// Setup the stack
	mov $0x7C00, %esp

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
