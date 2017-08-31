.code16
.global _start
.text

_start:
	
	cli
	hlt
	jmp -2


.data
string_0:
	.ascii " > Hello OS World!"
string_1:
	.ascii " > "
string_end:
