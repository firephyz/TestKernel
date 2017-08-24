.code16
.global _start
.text

_start:
	//mov $DATA_START, %ax

	xor %ax, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	lea string, %bp
	movb $0x00, %dh
	movb $0x00, %dl
	movb $0x13, %ah
	movb $0x01, %al
	movb $0x00, %bh
	movb $0x07, %bl
	movw $0x000C, %cx
	int $0x10
	
	cli
	hlt
	jmp -2

DATA_START: .data

string:
	.ascii "Hello World!"
