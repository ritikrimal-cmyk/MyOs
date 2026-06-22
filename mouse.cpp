#include "mouse.h"
#include "vbe.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

static MouseState state;
static uint8_t mouse_cycle = 0;
static signed char mouse_byte[3];

static void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" :: "a"(val), "Nd"(port));
}
static uint8_t inb(uint16_t port) {
    uint8_t val;
    asm volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) if (inb(0x64) & 1) return;
    } else {
        while (timeout--) if (!(inb(0x64) & 2)) return;
    }
}

static void mouse_write(uint8_t data) {
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, data);
}

static uint8_t mouse_read() {
    mouse_wait(0);
    return inb(0x60);
}

void mouse_init() {
    state.x = SCREEN_WIDTH  / 2;
    state.y = SCREEN_HEIGHT / 2;
    state.buttons = 0;

    // Enable auxiliary device
    mouse_wait(1);
    outb(0x64, 0xA8);

    // Enable interrupts
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    uint8_t status = inb(0x60) | 2;
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);

    // Use default settings
    mouse_write(0xF6);
    mouse_read();

    // Enable mouse
    mouse_write(0xF4);
    mouse_read();
}

void mouse_poll() {
    // Check if data is available
    uint8_t status = inb(0x64);
    if (!(status & 0x01)) return;  // no data
    if (!(status & 0x20)) return;  // not mouse data

    mouse_byte[mouse_cycle] = inb(0x60);
    mouse_cycle++;

    if (mouse_cycle == 3) {
        mouse_cycle = 0;

        // Validate first byte - bit 3 must always be set
        if (!(mouse_byte[0] & 0x08)) {
            mouse_cycle = 0;
            return;
        }

        state.buttons = mouse_byte[0] & 0x07;
        state.dx =  mouse_byte[1] - ((mouse_byte[0] & 0x10) ? 256 : 0);
        state.dy = -mouse_byte[2] + ((mouse_byte[0] & 0x20) ? 256 : 0);

        state.x += state.dx;
        state.y += state.dy;

        if (state.x < 0)              state.x = 0;
        if (state.y < 0)              state.y = 0;
        if (state.x >= SCREEN_WIDTH)  state.x = SCREEN_WIDTH  - 1;
        if (state.y >= SCREEN_HEIGHT) state.y = SCREEN_HEIGHT - 1;
    }
}
MouseState* mouse_get_state() { return &state; }
