#include "syscall.h"
#include "vga.h"
#include "memory.h"
#include "idt.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

static void print_number(uint32_t code) {
    char buf[12];
    int i = 0;
    if (code == 0) {
        buf[i++] = '0';
    } else {
        while (code > 0) {
            buf[i++] = '0' + (code % 10);
            code /= 10;
        }
        for (int a = 0, b = i - 1; a < b; a++, b--) {
            char tmp = buf[a];
            buf[a]   = buf[b];
            buf[b]   = tmp;
        }
    }
    buf[i] = '\0';
    vga_println(buf, COLOR_YELLOW);
}

extern "C" void syscall_handler(Registers regs) {
    switch (regs.eax) {

        case SYS_PRINT:
            if (regs.ebx)
                vga_println((const char*)regs.ebx, (uint8_t)regs.ecx);
            break;

        case SYS_MALLOC:
            // result returned in eax — caller reads it
            kmalloc(regs.ebx);
            break;

        case SYS_FREE:
            kfree((void*)regs.ebx);
            break;

        case SYS_EXIT:
            vga_print("Program exited with code: ", COLOR_YELLOW);
            print_number(regs.ebx);
            break;

        case SYS_CLEAR:
            vga_clear();
            break;

        default:
            vga_println("Unknown syscall!", COLOR_LIGHT_RED);
            break;
    }
}

extern "C" void syscall_stub();

void syscall_init() {
    idt_set_gate_user(0x80, (uint32_t)syscall_stub);
}
