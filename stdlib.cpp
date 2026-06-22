#include "stdlib.h"
#include "memory.h"

int atoi(const char* str) {
    int result = 0;
    int sign   = 1;
    int i      = 0;

    if (str[i] == '-') { sign = -1; i++; }
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return sign * result;
}

void itoa(int val, char* buf, int base) {
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

    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

void* malloc(size_t size) {
    return kmalloc(size);
}

void free(void* ptr) {
    kfree(ptr);
}
