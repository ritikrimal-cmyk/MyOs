#ifndef VBE_H
#define VBE_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define SCREEN_BPP    32

void     vbe_init_addr(uint32_t addr);
void     vbe_init();
void     vbe_putpixel(int x, int y, uint32_t color);
uint32_t vbe_getpixel(int x, int y);
void     vbe_clear(uint32_t color);
void     vbe_fill_rect(int x, int y, int w, int h, uint32_t color);
void     vbe_draw_char(int x, int y, char c, uint32_t fg, uint32_t bg);
void     vbe_draw_string(int x, int y, const char* str, uint32_t fg, uint32_t bg);

#define RGB(r,g,b)      ((uint32_t)(((r)<<16)|((g)<<8)|(b)))
#define VBE_BLACK       RGB(0,0,0)
#define VBE_WHITE       RGB(255,255,255)
#define VBE_RED         RGB(255,0,0)
#define VBE_GREEN       RGB(0,200,0)
#define VBE_BLUE        RGB(0,0,255)
#define VBE_GREY        RGB(128,128,128)
#define VBE_DGREY       RGB(64,64,64)
#define VBE_LGREY       RGB(192,192,192)
#define VBE_CYAN        RGB(0,255,255)
#define VBE_YELLOW      RGB(255,255,0)
#define VBE_ORANGE      RGB(255,128,0)

#endif
