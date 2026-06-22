#include "usermode.h"
#include "gdt.h"
#include "vga.h"

typedef unsigned int uint32_t;

// GDT selectors for user mode (Ring 3)
#define USER_CODE_SEG  0x1B  // 0x18 | 0x3 (Ring 3)
#define USER_DATA_SEG  0x23  // 0x20 | 0x3 (Ring 3)

void usermode_init() {
    vga_println("  User mode segments configured", COLOR_LIGHT_GREY);
}

void usermode_jump(uint32_t entry, uint32_t user_stack) {
    // Use iret to switch to Ring 3
    asm volatile(
        "cli\n"
        "mov %1, %%eax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        // Push user mode stack frame for iret
        "push %1\n"         // ss (user data segment)
        "push %2\n"         // esp (user stack)
        "pushf\n"           // eflags
        "pop %%eax\n"
        "or $0x200, %%eax\n" // enable interrupts in user mode
        "push %%eax\n"
        "push %0\n"         // cs (user code segment)
        "push %3\n"         // eip (entry point)
        "iret\n"
        :: "i"(USER_CODE_SEG),
           "i"(USER_DATA_SEG),
           "r"(user_stack),
           "r"(entry)
    );
}
