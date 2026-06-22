#include "gdt.h"

#define GDT_ENTRIES 3

static GDTEntry gdt[GDT_ENTRIES];
static GDTPointer gdt_ptr;

static void gdt_set_entry(int num, uint32_t base, uint32_t limit,
                           uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[num].access      = access;
}

void gdt_init() {
    gdt_ptr.limit = (sizeof(GDTEntry) * GDT_ENTRIES) - 1;
    gdt_ptr.base  = (uint32_t)&gdt;

    // Entry 0 — Null descriptor (required by CPU)
    gdt_set_entry(0, 0, 0, 0, 0);

    // Entry 1 — Kernel Code segment
    // access 0x9A = present, ring 0, code, executable, readable
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Entry 2 — Kernel Data segment
    // access 0x92 = present, ring 0, data, writable
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Load the GDT
    asm volatile("lgdt %0" : : "m"(gdt_ptr));

    // Reload segment registers
    asm volatile(
        "mov $0x10, %ax\n"   // 0x10 = second entry = kernel data
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "mov %ax, %ss\n"
        "ljmp $0x08, $1f\n"  // 0x08 = first entry = kernel code
        "1:\n"
    );
}
