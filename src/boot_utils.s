.code16

.text
_print_registers:
	push %ax
	push %bx
	push %cx
	push %dx
	push %si
	push %di
	push %bp
	push %sp
	push %cs
	push %ds
	push %es
	push %ss

	// Save  address of the top of all these registers
	mov %sp, %bp
	add $22, %bp // for the (12 registers on the stack - 1)* 2 bytes each. Must start from the first
	push %bp

	lea _print_registers_template, %bp
	add $-12, %sp // Reserve space for register offset indexes in the _print_registers_template

	// Store the register offset indexes in the space we just reserved on stack
	mov $0, %si
	movl $0x030B131B, (%bp, %si, 1)
	mov $4, %si
	movl $0x222A323A, (%bp, %si, 1)
	mov $8, %si
	movl $0x41495159, (%bp, %si, 1)

	// Register we are printing. Also offset index pointing in the register offset indexes
	mov $0, %si

_print_register_loop:
	xor %ax, %ax

	// Get address of register offset indexes array into bp
	push %bp
	mov %sp, %bp
	add $2, %bp
	mov (%bp, %si, 1), %al // Get the next index from the array
	pop %bp

	// Use that index to construct the address we need to store the register value string in
	push %si
	push %bp
	mov %ax, %si
	lea (%bp, %si, 1), %bp // bp contains the destination address mentioned above

	mov %sp, %bx
	mov 2(%bx), %si // Get address of value of current register
	push %bp
	mov %si, %bp
	mov (%bp), %dx // dx now holds value of the target register
	pop %bp
	push %bx
	call _number_to_string // Convert that register value into a string and store it in the print register template

	pop %bx
	sub $2, 2(%bx) // Increment address to next register for next loop
	pop %bp // Restore  
	pop %si // Restore register number we are printing
	inc %si

	cmp $12, %si
	jne _print_register_loop // Get next register value and convert to string

	// Print the final result
	mov $0x07, %bl

	lea _print_registers_template, %ax
	mov $31, %cx
	mov $0x0000, %dx
	call _print_string

	lea _print_registers_template + 31, %ax
	mov $31, %cx
	mov $0x0100, %dx
	call _print_string

	lea _print_registers_template + 62, %ax
	mov $31, %cx
	mov $0x0200, %dx
	call _print_string

	pop %bp
	push %ss
	push %es
	push %ds
	push %cs
	push %sp
	push %bp
	push %di
	push %si
	push %dx
	push %cx
	push %bx
	push %ax
	ret


// _print_number:
// 	push %bp

// 	add $-4, %sp
// 	mov %sp, %bp
// 	push %dx // Save row and column
// 	mov %ax, %dx
// 	call _number_to_string

// 	mov %bp, %ax
// 	mov $0x07, %bl
// 	mov $4, %cx
// 	pop %dx // Restore row and column
// 	call _print_string

// 	pop %bp
// 	ret

	// @dx holds number
	// @bp address to output string buffer
_number_to_string:
	push %si
	push %di
	mov $0, %di // Output string offset index

_number_to_string_loop:
	// Move the nib we are interested in into al
	cmp $0, %di
	jne _number_to_string_try_nib_1
	xor %ax, %ax
	mov %dh, %al // Extract first nib
	shr $4, %al
	jmp _number_to_string_convert_to_char
_number_to_string_try_nib_1:
	cmp $1, %di
	jne _number_to_string_try_nib_2
	xor %ax, %ax
	mov %dh, %al
	and $0xF, %ax
	jmp _number_to_string_convert_to_char
_number_to_string_try_nib_2:
	cmp $2, %di
	jne _number_to_string_try_nib_3
	xor %ax, %ax
	mov %dl, %al // Extract third nib
	shr $4, %al
	jmp _number_to_string_convert_to_char
_number_to_string_try_nib_3:
	mov %dl, %al
	and $0xF, %ax

_number_to_string_convert_to_char:
	cmp $0xA, %al
	jl _number_to_string_0_9
	push %bp
	sub $10, %al
	lea _number_to_string_hex_chars, %bp
	mov %ax, %si
	mov (%bp, %si, 1), %al // Holds hex char to put in output string
	pop %bp // Get output string address back
	jmp _number_to_string_store_char
_number_to_string_0_9:
	add $'0, %al

	// %al holds the character to print for this nib
_number_to_string_store_char:
	mov %al, (%bp, %di, 1)
	inc %di

	// Go back and print the next char if we need to
	cmp $4, %di
	jne _number_to_string_loop

	pop %di
	pop %si
	ret

.data
_number_to_string_hex_chars:
	.ascii "ABCDEF"
_print_registers_template:
	.ascii "AX=#### BX=#### CX=#### DX=####SI=#### DI=#### BP=#### SP=####CS=#### DS=#### ES=#### SS=####"


	// @dx holds number
	// @bp address to output string buffer
_number_to_string:
	push %si
	push %di
	mov $0, %di // Output string offset index

_number_to_string_loop:
	// Move the nib we are interested in into al
	cmp $0, %di
	jne _number_to_string_try_nib_1
	xor %ax, %ax
	mov %dh, %al // Extract first nib
	shr $4, %al
	jmp _number_to_string_convert_to_char
_number_to_string_try_nib_1:
	cmp $1, %di
	jne _number_to_string_try_nib_2
	xor %ax, %ax
	mov %dh, %al
	and $0xF, %ax
	jmp _number_to_string_convert_to_char
_number_to_string_try_nib_2:
	cmp $2, %di
	jne _number_to_string_try_nib_3
	xor %ax, %ax
	mov %dl, %al // Extract third nib
	shr $4, %al
	jmp _number_to_string_convert_to_char
_number_to_string_try_nib_3:
	mov %dl, %al
	and $0xF, %ax

_number_to_string_convert_to_char:
	cmp $0xA, %al
	jl _number_to_string_0_9
	push %bp
	sub $10, %al
	lea _number_to_string_hex_chars, %bp
	mov %ax, %si
	mov (%bp, %si, 1), %al // Holds hex char to put in output string
	pop %bp // Get output string address back
	jmp _number_to_string_store_char
_number_to_string_0_9:
	add $'0, %al

	// %al holds the character to print for this nib
_number_to_string_store_char:
	mov %al, (%bp, %di, 1)
	inc %di

	// Go back and print the next char if we need to
	cmp $4, %di
	jne _number_to_string_loop

	pop %di
	pop %si
	ret

.data
_number_to_string_hex_chars:
	.ascii "ABCDEF"
_drive_string:
	.ascii "H:0x#### C:0x#### S:0x####              "