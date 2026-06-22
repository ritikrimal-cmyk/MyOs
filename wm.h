#ifndef WM_H
#define WM_H

typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;

#define MAX_WINDOWS  8
#define TITLE_HEIGHT 20

struct Window {
    int      x, y, w, h;
    char     title[32];
    uint8_t  visible;
    uint8_t  focused;
    void (*draw_content)(int x, int y, int w, int h);
};

void wm_init();
int  wm_create(const char* title, int x, int y, int w, int h,
               void (*draw_content)(int,int,int,int));
void wm_draw_all();
void wm_handle_mouse(int mx, int my, uint8_t buttons);
void wm_close(int id);

#endif
