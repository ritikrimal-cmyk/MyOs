#ifndef VGA_H
#define VGA_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Colors
#define COLOR_BLACK        0x0
#define COLOR_BLUE         0x1
#define COLOR_GREEN        0x2
#define COLOR_CYAN         0x3
#define COLOR_RED          0x4
#define COLOR_MAGENTA      0x5
#define COLOR_BROWN        0x6
#define COLOR_LIGHT_GREY   0x7
#define COLOR_DARK_GREY    0x8
#define COLOR_LIGHT_BLUE   0x9
#define COLOR_LIGHT_GREEN  0xA
#define COLOR_LIGHT_CYAN   0xB
#define COLOR_LIGHT_RED    0xC
#define COLOR_LIGHT_MAGENTA 0xD
#define COLOR_YELLOW       0xE
#define COLOR_WHITE        0xF

void vga_init();
void vga_clear();
void vga_putchar(char c, uint8_t color);
void vga_print(const char* str, uint8_t color);
void vga_println(const char* str, uint8_t color);
void vga_set_cursor(int pos);
int  vga_get_cursor();
void vga_scroll();

#endif
