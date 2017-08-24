SECTIONS
{
	. = 0x7c00;
	.text : { *(.text) }
	.data : { *(.data) }

	. = 0x7c00 + 0x1FE;
	.magic : { SHORT(0xAA55) }
}