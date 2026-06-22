#include "desktop.h"
#include "vbe.h"
#include "wm.h"
#include "mouse.h"
#include "keyboard.h"
#include "string.h"

typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;

static int active_window = 0;
static int prev_mx = -1;
static int prev_my = -1;

static void draw_cursor(int x, int y) {
    vbe_fill_rect(x,     y,     2, 12, VBE_WHITE);
    vbe_fill_rect(x,     y,     8,  2, VBE_WHITE);
    vbe_fill_rect(x + 1, y + 1, 1, 10, VBE_BLACK);
    vbe_fill_rect(x + 1, y + 1, 6,  1, VBE_BLACK);
}

static void draw_taskbar() {
    int y = SCREEN_HEIGHT - 30;
    vbe_fill_rect(0, y, SCREEN_WIDTH, 30, RGB(30,30,30));
    vbe_fill_rect(0, y, SCREEN_WIDTH,  1, RGB(80,80,80));
    vbe_fill_rect(4, y + 4, 60, 22, RGB(0,120,215));
    vbe_draw_string(10, y + 10, "MyOS", VBE_WHITE, RGB(0,120,215));
    vbe_draw_string(SCREEN_WIDTH - 200, y + 10,
        "F1=Terminal F2=About ESC=Shell",
        VBE_LGREY, RGB(30,30,30));
}

static void draw_help() {
    vbe_draw_string(10, SCREEN_HEIGHT - 60,
        "F1=Terminal  F2=About  F3=Close  ESC=Shell",
        VBE_YELLOW, RGB(0,80,140));
}

static void terminal_content(int x, int y, int w, int h) {
    vbe_fill_rect(x, y, w, h, RGB(20,20,20));
    vbe_draw_string(x + 4, y + 4,  "MyOS Terminal v1.0",  RGB(0,200,0),  RGB(20,20,20));
    vbe_draw_string(x + 4, y + 16, "Type commands below",  VBE_LGREY,    RGB(20,20,20));
    vbe_draw_string(x + 4, y + 32, "> _",                  VBE_WHITE,    RGB(20,20,20));
}

static void about_content(int x, int y, int w, int h) {
    vbe_fill_rect(x, y, w, h, VBE_WHITE);
    vbe_draw_string(x + 20, y + 20, "MyOS v1.0",       RGB(0,0,0),    VBE_WHITE);
    vbe_draw_string(x + 20, y + 36, "A bare metal OS", RGB(80,80,80), VBE_WHITE);
    vbe_draw_string(x + 20, y + 52, "Built with C++",  RGB(80,80,80), VBE_WHITE);
    vbe_draw_string(x + 20, y + 68, "and Assembly",    RGB(80,80,80), VBE_WHITE);
    vbe_draw_string(x + 20, y + 90, "Press F3 to close", RGB(0,120,215), VBE_WHITE);
}

static void redraw() {
    vbe_clear(RGB(0, 80, 140));
    for (int y = 0; y < SCREEN_HEIGHT - 30; y += 40)
        for (int x = 0; x < SCREEN_WIDTH; x += 40)
            vbe_putpixel(x, y, RGB(0, 90, 155));
    draw_taskbar();
    draw_help();
    wm_draw_all();
}

void desktop_init() {
    mouse_init();
    wm_init();
    redraw();
}

void desktop_run() {
    while (1) {
        // Read keyboard
        char c = keyboard_getchar();

        if (c) {
            // F1 = open terminal
            if (c == (char)0x81) {
                wm_create("Terminal", 50, 50, 400, 200, terminal_content);
                redraw();
            }
            // F2 = open about
            else if (c == (char)0x82) {
                wm_create("About", 200, 100, 280, 160, about_content);
                redraw();
            }
            // F3 = close active window
            else if (c == (char)0x83) {
                wm_close(active_window);
                redraw();
            }
            // ESC = go back to shell
            else if (c == 27) {
                return;
            }
        }

        for (volatile int i = 0; i < 10000; i++);
    }
}
