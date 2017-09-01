.code16

.text
_start:
	xor %ax, %ax
loop_start:
	add $1, %ax
	cmp $0, %ax
	jne loop_start
	
	cli
halt:
	hlt
	jmp halt


.data
