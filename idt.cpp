#include "idt.h"
#include "timer.h"
extern "C" void timer_handler();

#define IDT_ENTRIES 256

static IDTEntry idt[IDT_ENTRIES];
static IDTPointer idt_ptr;

static void idt_set_gate(uint8_t num, uint32_t base,
                          uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = sel;
    idt[num].zero      = 0;
    idt[num].flags     = flags;
}

extern "C" {
    void isr0();  void isr1();  void isr2();  void isr3();
    void isr4();  void isr5();  void isr6();  void isr7();
    void isr8();  void isr9();  void isr10(); void isr11();
    void isr12(); void isr13(); void isr14(); void isr15();
    void isr16(); void isr17(); void isr18(); void isr19();
    void irq0();  void irq1();
}

extern "C" void keyboard_handler();

// Write byte to port
static void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" :: "a"(val), "Nd"(port));
}

// Read byte from port
static uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void idt_init() {
    idt_ptr.limit = (sizeof(IDTEntry) * IDT_ENTRIES) - 1;
    idt_ptr.base  = (uint32_t)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_gate(i, 0, 0, 0);

    // Remap PIC
    outb(0x20, 0x11);  // Init master PIC
    outb(0xA0, 0x11);  // Init slave PIC
    outb(0x21, 0x20);  // Master IRQ offset = 32
    outb(0xA1, 0x28);  // Slave IRQ offset  = 40
    outb(0x21, 0x04);  // Master has slave at IRQ2
    outb(0xA1, 0x02);  // Slave cascade identity
    outb(0x21, 0x01);  // 8086 mode master
    outb(0xA1, 0x01);  // 8086 mode slave

    // Unmask only IRQ0 (timer) and IRQ1 (keyboard)
    // 0xFC = 11111100 — unmask IRQ0 and IRQ1
    outb(0x21, 0xFC);
    outb(0xA1, 0xFF);  // mask all slave IRQs

    // CPU exceptions
    idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);
    idt_set_gate(1,  (uint32_t)isr1,  0x08, 0x8E);
    idt_set_gate(2,  (uint32_t)isr2,  0x08, 0x8E);
    idt_set_gate(3,  (uint32_t)isr3,  0x08, 0x8E);
    idt_set_gate(4,  (uint32_t)isr4,  0x08, 0x8E);
    idt_set_gate(5,  (uint32_t)isr5,  0x08, 0x8E);
    idt_set_gate(6,  (uint32_t)isr6,  0x08, 0x8E);
    idt_set_gate(7,  (uint32_t)isr7,  0x08, 0x8E);
    idt_set_gate(8,  (uint32_t)isr8,  0x08, 0x8E);
    idt_set_gate(9,  (uint32_t)isr9,  0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);

    // Hardware IRQs
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E); // Timer
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E); // Keyboard

    asm volatile("lidt %0" : : "m"(idt_ptr));
    asm volatile("sti");
}

extern "C" void isr_handler(Registers regs) {
    const char* exceptions[] = {
        "Division By Zero", "Debug", "NMI", "Breakpoint",
        "Overflow", "Bound Range", "Invalid Opcode", "FPU Not Available",
        "Double Fault", "Coprocessor Overrun", "Invalid TSS", "Segment Not Present",
        "Stack Fault", "General Protection", "Page Fault", "Reserved",
        "FPU Exception", "Alignment Check", "Machine Check", "SIMD Exception"
    };

    unsigned short* screen = (unsigned short*)0xB8000;
    const char* msg = "EXCEPTION: ";
    int pos = 80 * 12;
    for (int i = 0; msg[i]; i++)
        screen[pos++] = (unsigned short)msg[i] | (0x4F << 8);

    if (regs.int_no < 20) {
        const char* name = exceptions[regs.int_no];
        for (int i = 0; name[i]; i++)
            screen[pos++] = (unsigned short)name[i] | (0x4F << 8);
    }

    while(1);
}

extern "C" void irq_handler(Registers regs) {
    // IRQ0 = timer (interrupt 32)
    if (regs.int_no == 32)
        timer_handler();

    // IRQ1 = keyboard (interrupt 33)
    if (regs.int_no == 33)
        keyboard_handler();

    // Send EOI
    if (regs.int_no >= 40)
        outb(0xA0, 0x20);
    outb(0x20, 0x20);
}
void idt_set_gate_user(uint8_t num, uint32_t base) {
    // 0xEE = present, ring 3, 32-bit interrupt gate
    idt_set_gate(num, base, 0x08, 0xEE);
}
