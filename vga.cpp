#include "vga.h"

static unsigned short* screen = (unsigned short*)VGA_MEMORY;
static int cursor = 0;

// Hardware cursor via VGA ports
static void update_hw_cursor(int pos) {
    asm volatile("outb %0, %1" :: "a"((uint8_t)0x0F), "Nd"((uint16_t)0x3D4));
    asm volatile("outb %0, %1" :: "a"((uint8_t)(pos & 0xFF)), "Nd"((uint16_t)0x3D5));
    asm volatile("outb %0, %1" :: "a"((uint8_t)0x0E), "Nd"((uint16_t)0x3D4));
    asm volatile("outb %0, %1" :: "a"((uint8_t)((pos >> 8) & 0xFF)), "Nd"((uint16_t)0x3D5));
}

void vga_init() {
    vga_clear();
}

void vga_clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        screen[i] = 0x0000;
    cursor = 0;
    update_hw_cursor(0);
}

// Scroll screen up by one line
void vga_scroll() {
    // Move every line up by one
    for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++)
        screen[i] = screen[i + VGA_WIDTH];

    // Clear the last line
    for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++)
        screen[i] = 0x0000;

    cursor -= VGA_WIDTH;
}

void vga_putchar(char c, uint8_t color) {
    if (c == '\n') {
        // Move to next line
        cursor += VGA_WIDTH - (cursor % VGA_WIDTH);
    } else if (c == '\r') {
        cursor -= cursor % VGA_WIDTH;
    } else if (c == '\b') {
        if (cursor > 0) {
            cursor--;
            screen[cursor] = 0x0000;
        }
    } else if (c == '\t') {
        // Tab = 4 spaces
        int spaces = 4 - (cursor % 4);
        for (int i = 0; i < spaces; i++)
            vga_putchar(' ', color);
        return;
    } else {
        screen[cursor++] = (unsigned short)c | ((unsigned short)color << 8);
    }

    // Scroll if we reached the bottom
    if (cursor >= VGA_WIDTH * VGA_HEIGHT)
        vga_scroll();

    update_hw_cursor(cursor);
}

void vga_print(const char* str, uint8_t color) {
    for (int i = 0; str[i]; i++)
        vga_putchar(str[i], color);
}

void vga_println(const char* str, uint8_t color) {
    vga_print(str, color);
    vga_putchar('\n', color);
}

void vga_set_cursor(int pos) {
    cursor = pos;
    update_hw_cursor(pos);
}

int vga_get_cursor() {
    return cursor;
}
