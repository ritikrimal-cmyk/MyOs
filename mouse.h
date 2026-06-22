#ifndef MOUSE_H
#define MOUSE_H

typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;

struct MouseState {
    int x, y;
    int dx, dy;
    uint8_t buttons; // bit 0=left, bit 1=right, bit 2=middle
};

void        mouse_init();
void        mouse_poll();
MouseState* mouse_get_state();

#endif
