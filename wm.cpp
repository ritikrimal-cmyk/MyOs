#include "wm.h"
#include "vbe.h"
#include "string.h"

typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;

static Window windows[MAX_WINDOWS];
static int    win_count  = 0;
static int    drag_id    = -1;
static int    drag_off_x = 0;
static int    drag_off_y = 0;
static uint8_t last_buttons = 0;

void wm_init() {
    for (int i = 0; i < MAX_WINDOWS; i++)
        windows[i].visible = 0;
}

int wm_create(const char* title, int x, int y, int w, int h,
              void (*draw_content)(int,int,int,int)) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (!windows[i].visible) {
            windows[i].x        = x;
            windows[i].y        = y;
            windows[i].w        = w;
            windows[i].h        = h;
            windows[i].visible  = 1;
            windows[i].focused  = 1;
            windows[i].draw_content = draw_content;
            strncpy(windows[i].title, title, 31);
            win_count++;
            return i;
        }
    }
    return -1;
}

static void wm_draw_window(int i) {
    Window* w = &windows[i];
    if (!w->visible) return;

    uint32_t title_color = w->focused ? RGB(0,100,200) : RGB(80,80,80);

    // Shadow
    vbe_fill_rect(w->x + 4, w->y + 4, w->w, w->h + TITLE_HEIGHT, RGB(30,30,30));

    // Title bar
    vbe_fill_rect(w->x, w->y, w->w, TITLE_HEIGHT, title_color);

    // Title text
    vbe_draw_string(w->x + 6, w->y + 6, w->title, VBE_WHITE, title_color);

    // Close button
    vbe_fill_rect(w->x + w->w - 18, w->y + 3, 14, 14, RGB(200,50,50));
    vbe_draw_string(w->x + w->w - 15, w->y + 5, "X", VBE_WHITE, RGB(200,50,50));

    // Window body
    vbe_fill_rect(w->x, w->y + TITLE_HEIGHT, w->w, w->h, RGB(240,240,240));

    // Border
    for (int dx = 0; dx < w->w; dx++) {
        vbe_putpixel(w->x + dx, w->y, RGB(150,150,150));
        vbe_putpixel(w->x + dx, w->y + TITLE_HEIGHT + w->h, RGB(150,150,150));
    }
    for (int dy = 0; dy < w->h + TITLE_HEIGHT; dy++) {
        vbe_putpixel(w->x,          w->y + dy, RGB(150,150,150));
        vbe_putpixel(w->x + w->w-1, w->y + dy, RGB(150,150,150));
    }

    // Draw content
    if (w->draw_content)
        w->draw_content(w->x + 4, w->y + TITLE_HEIGHT + 4,
                        w->w - 8, w->h - 8);
}

void wm_draw_all() {
    for (int i = 0; i < MAX_WINDOWS; i++)
        if (windows[i].visible)
            wm_draw_window(i);
}

void wm_handle_mouse(int mx, int my, uint8_t buttons) {
    uint8_t clicked = buttons & ~last_buttons; // newly pressed
    last_buttons = buttons;

    // Dragging
    if (buttons & 1) {
        if (drag_id >= 0) {
            windows[drag_id].x = mx - drag_off_x;
            windows[drag_id].y = my - drag_off_y;
            return;
        }
    } else {
        drag_id = -1;
    }

    if (clicked & 1) {
        // Check each window title bar for click
        for (int i = MAX_WINDOWS - 1; i >= 0; i--) {
            Window* w = &windows[i];
            if (!w->visible) continue;

            // Close button
            if (mx >= w->x + w->w - 18 && mx <= w->x + w->w - 4 &&
                my >= w->y + 3 && my <= w->y + 17) {
                wm_close(i);
                return;
            }

            // Title bar drag
            if (mx >= w->x && mx <= w->x + w->w &&
                my >= w->y && my <= w->y + TITLE_HEIGHT) {
                drag_id    = i;
                drag_off_x = mx - w->x;
                drag_off_y = my - w->y;

                // Focus this window
                for (int j = 0; j < MAX_WINDOWS; j++)
                    windows[j].focused = 0;
                w->focused = 1;
                return;
            }
        }
    }
}

void wm_close(int id) {
    if (id >= 0 && id < MAX_WINDOWS) {
        windows[id].visible = 0;
        win_count--;
    }
}
