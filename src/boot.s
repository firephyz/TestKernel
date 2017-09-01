.code16
.global gdtr_value

.text
_start:
	cli
	lgdt gdtr_value
	hlt
	jmp -2

.data

.section gdt_section, "wa"
	// Null descriptor
	.long 0x00000000
	.long 0x00000000

	// Code descriptor
	.long 0x0000FFFF
	.long 0x00CF9A00

	// Data descriptor
	.long 0x0000FFFF
	.long 0x00CF9200

	// Make the remaining 5 descriptors 0
	.fill 5, 4, 0

.section idt_section, "wa"

	.long 0x12345678
