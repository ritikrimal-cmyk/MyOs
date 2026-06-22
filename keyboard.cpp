#include "keyboard.h"
#include "idt.h"

#define KEYBOARD_PORT 0x60

// Special key codes
#define KEY_F1  0x81
#define KEY_F2  0x82
#define KEY_F3  0x83
#define KEY_F4  0x84
#define KEY_ESC 0x1B

// Scancodes for modifier keys
#define SC_LSHIFT  0x2A
#define SC_RSHIFT  0x36
#define SC_LSHIFT_R 0xAA
#define SC_RSHIFT_R 0xB6
#define SC_CAPS    0x3A

// Normal keymap
static char keymap[] = {
    0,   27,  '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q', 'w','e','r','t','y','u','i','o','p','[',']','\n',
    0,   'a', 's','d','f','g','h','j','k','l',';','\'','`',
    0,   '\\','z','x','c','v','b','n','m',',','.','/',0,
    '*', 0,   ' '
};

// Shifted keymap
static char keymap_shift[] = {
    0,   27,  '!','@','#','$','%','^','&','*','(',')','-','+','\b',
    '\t','Q', 'W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,   'A', 'S','D','F','G','H','J','K','L',':','"', '~',
    0,   '|', 'Z','X','C','V','B','N','M','<','>','?', 0,
    '*', 0,   ' '
};

static char key_buffer[256];
static int  buf_start  = 0;
static int  buf_end    = 0;
static int  shift_held = 0;
static int  caps_lock  = 0;

static uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

extern "C" void keyboard_handler() {
    uint8_t scancode = inb(KEYBOARD_PORT);

    // Handle shift press/release
    if (scancode == SC_LSHIFT || scancode == SC_RSHIFT) {
        shift_held = 1;
        return;
    }
    if (scancode == SC_LSHIFT_R || scancode == SC_RSHIFT_R) {
        shift_held = 0;
        return;
    }

    // Handle caps lock toggle
    if (scancode == SC_CAPS) {
        caps_lock = !caps_lock;
        return;
    }

    // Only handle key press (not release)
    if (!(scancode & 0x80)) {
        char c = 0;

        // F1-F4 keys
        if      (scancode == 0x3B) c = (char)0x81;
        else if (scancode == 0x3C) c = (char)0x82;
        else if (scancode == 0x3D) c = (char)0x83;
        else if (scancode == 0x3E) c = (char)0x84;
        // Arrow keys
        else if (scancode == 0x48) c = (char)0x90;
        else if (scancode == 0x50) c = (char)0x91;
        else if (scancode == 0x4B) c = (char)0x92;
        else if (scancode == 0x4D) c = (char)0x93;
        // Regular keys
        else if (scancode < sizeof(keymap)) {
            if (shift_held) {
                c = keymap_shift[scancode];
            } else {
                c = keymap[scancode];
                // Apply caps lock to letters only
                if (caps_lock && c >= 'a' && c <= 'z')
                    c = c - 'a' + 'A';
            }
        }

        if (c) {
            key_buffer[buf_end] = c;
            buf_end = (buf_end + 1) % 256;
        }
    }
}

void keyboard_init() {
    shift_held = 0;
    caps_lock  = 0;
}

char keyboard_getchar() {
    if (buf_start == buf_end) return 0;
    char c = key_buffer[buf_start];
    buf_start = (buf_start + 1) % 256;
    return c;
}
