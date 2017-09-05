#ifndef _KERNEL_INCLUDED
#define _KERNEL_INCLUDED

#include <stdint.h>
#include <stddef.h>

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25

#define VGA_COLOR(fg, bg) fg | (bg << 4)

extern void * handle_int_08;
extern void * handle_int_09;
extern void * handle_int_xx;
extern uint32_t CODE_SELECTOR;

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

struct console {
	uint8_t x_pos;
	uint8_t y_pos;
	uint8_t color;
	uint16_t * buffer;
};

struct idt_descriptor {
	uint32_t first;
	uint32_t second;
};

struct idt_ptr_t {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

struct keyboard {
	int start;
	int end;
	uint8_t buffer[256];
};

struct idt_descriptor create_idt_entry(uint16_t selector, uint32_t offset);
void fill_idt_table();
void console_init();
void console_clear_screen();
static inline void console_check_bounds();
static inline uint16_t set_vga_entry(char character);
size_t strlen(char * string);
void console_write_string(char * string);
void console_putchar(char character);
void console_write_number(unsigned int num);
char getc();

static inline uint8_t inb(uint16_t port);
static inline void outb(uint16_t port, uint8_t value);

#endif