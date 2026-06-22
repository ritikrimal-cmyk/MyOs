#include "stdio.h"
#include "string.h"
#include "vga.h"

typedef unsigned int  uint32_t;
typedef unsigned char uint8_t;

// Simple itoa helper
static void itoa(int val, char* buf, int base) {
    if (val == 0) { buf[0] = '0'; buf[1] = '\0'; return; }

    char tmp[32];
    int  i   = 0;
    int  neg = 0;

    if (val < 0 && base == 10) { neg = 1; val = -val; }

    unsigned int uval = (unsigned int)val;
    while (uval > 0) {
        int r = uval % base;
        tmp[i++] = r < 10 ? '0' + r : 'A' + r - 10;
        uval /= base;
    }
    if (neg) tmp[i++] = '-';

    // Reverse
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

int sprintf(char* buf, const char* fmt, ...) {
    // Simple va_args implementation
    unsigned int* args = (unsigned int*)(&fmt) + 1;
    int arg_idx = 0;
    int pos = 0;

    for (int i = 0; fmt[i]; i++) {
        if (fmt[i] != '%') {
            buf[pos++] = fmt[i];
            continue;
        }
        i++;
        switch (fmt[i]) {
            case 'd': {
                char tmp[32];
                itoa((int)args[arg_idx++], tmp, 10);
                for (int j = 0; tmp[j]; j++) buf[pos++] = tmp[j];
                break;
            }
            case 'x': {
                char tmp[32];
                itoa((int)args[arg_idx++], tmp, 16);
                for (int j = 0; tmp[j]; j++) buf[pos++] = tmp[j];
                break;
            }
            case 's': {
                char* s = (char*)args[arg_idx++];
                for (int j = 0; s[j]; j++) buf[pos++] = s[j];
                break;
            }
            case 'c':
                buf[pos++] = (char)args[arg_idx++];
                break;
            case '%':
                buf[pos++] = '%';
                break;
        }
    }
    buf[pos] = '\0';
    return pos;
}

void printf(const char* fmt, ...) {
    char buf[512];
    // reuse sprintf
    unsigned int* args = (unsigned int*)(&fmt) + 1;
    unsigned int* fakeargs = args;
    (void)fakeargs;

    // Call sprintf with same args
    sprintf(buf, fmt);
    vga_print(buf, COLOR_WHITE);
}
