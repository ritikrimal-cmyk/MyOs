#include "timer.h"
#include "vga.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

static uint32_t ticks = 0;

// Called by IDT every timer interrupt
extern "C" void timer_handler() {
    ticks++;
}

void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    // Command port: channel 0, lobyte/hibyte, rate generator
    asm volatile("outb %0, %1" :: "a"((uint8_t)0x36), "Nd"((uint16_t)0x43));

    // Send divisor low byte then high byte
    asm volatile("outb %0, %1" :: "a"((uint8_t)(divisor & 0xFF)),        "Nd"((uint16_t)0x40));
    asm volatile("outb %0, %1" :: "a"((uint8_t)((divisor >> 8) & 0xFF)), "Nd"((uint16_t)0x40));
}

uint32_t timer_get_ticks() {
    return ticks;
}
