.code16
.global _start
.text

_start:
	mov $0xB800, %ax 	// Set up data segment as we will be using that
	mov %ax, %ds	 	// with the indirect addressing from %di
	mov $0x0000, %di

	mov $0x07, %dh 	// Set print attribute
print_loop:
	mov $' , %dl 	// Fill with spaces
	mov %dx, (%di)	// Move data to text memory

	add $2, %di		// Increment to the next character
	mov %di, %ax
	cmp $4000, %ax
	jne print_loop	// Jump back until screen is full

	// Print the famous message using bios
	mov $0x13, %ah
	mov $0x00, %al
	mov $0x00, %bh
	mov $0x07, %bl
	mov $string_1 - string_0, %cx
	mov $0x00, %dh
	mov $0x00, %dl
	mov $string_0, %bp
	int $0x10

	mov $string_end - string_1, %cx
	mov $0x01, %dh
	mov $string_1, %bp
	int $0x10

	// Move cursor
	mov $0x02, %ah
	mov $0x00, %bh
	mov $0x01, %dh
	mov $0x03, %dl
	int $0x10
	
	cli
	hlt
	jmp -2


.data
string_0:
	.ascii " > Hello OS World!"
string_1:
	.ascii " > "
string_end:
