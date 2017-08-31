ENTRY(_start)
SECTIONS
{
	. = 0x7c00;
	.text : { build/boot.o (.text) }
	.data : { build/boot.o (.data) }

	. = 0x7c00 + 0x1FE;
	.magic : { SHORT(0xAA55) }

	.os_text : { build/kernel.o (.text) }
	.os_data : { build/kernel.o (.data) }
}