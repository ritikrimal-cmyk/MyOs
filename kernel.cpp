#include "memory.h"
#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "shell.h"
#include "vga.h"
#include "paging.h"
#include "syscall.h"
#include "fs.h"
#include "timer.h"
#include "process.h"
#include "elf.h"
#include "usermode.h"
#include "mmap.h"
#include "vbe.h"
#include "desktop.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

void delay() {
    for (volatile int i = 0; i < 10000000; i++);
}

extern "C" void kernelMain(void* multiboot_structure, unsigned int magic) {
    vga_init();
    vga_println("Step 1: VGA OK", COLOR_LIGHT_GREEN);
    delay();

    gdt_init();
    vga_println("Step 2: GDT OK", COLOR_LIGHT_GREEN);
    delay();

    idt_init();
    vga_println("Step 3: IDT OK", COLOR_LIGHT_GREEN);
    delay();

    memory_init();
    vga_println("Step 4: Memory OK", COLOR_LIGHT_GREEN);
    delay();

    fs_init();
    vga_println("Step 5: FS OK", COLOR_LIGHT_GREEN);
    delay();

    paging_init();
    vga_println("Step 6: Paging OK", COLOR_LIGHT_GREEN);
    delay();

    keyboard_init();
    vga_println("Step 7: Keyboard OK", COLOR_LIGHT_GREEN);
    delay();

    syscall_init();
    vga_println("Step 8: Syscalls OK", COLOR_LIGHT_GREEN);
    delay();

    timer_init(100);
    vga_println("Step 9: Timer OK", COLOR_LIGHT_GREEN);
    delay();

    process_init();
    vga_println("Step 10: Scheduler OK", COLOR_LIGHT_GREEN);
    delay();

    usermode_init();
    vga_println("Step 11: User Mode OK", COLOR_LIGHT_GREEN);
    delay();

    mmap_init();
    vga_println("Step 12: Memory Isolation OK", COLOR_LIGHT_GREEN);
    delay();

   vga_println("", COLOR_WHITE);
    vga_println("All systems ready! Starting shell...", COLOR_YELLOW);
    delay();
    delay();
    shell_init();
    shell_run();
}

