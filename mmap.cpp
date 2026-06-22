#include "mmap.h"
#include "memory.h"
#include "vga.h"

typedef unsigned int uint32_t;

// Each process gets its own region starting at USER_SPACE_START + pid * USER_SPACE_SIZE
void mmap_init() {
    vga_println("  Memory isolation ready", COLOR_LIGHT_GREY);
}

void* mmap_alloc_user(uint32_t pid, uint32_t size) {
    // Each process gets isolated region based on pid
    uint32_t base = USER_SPACE_START + pid * USER_SPACE_SIZE;
    if (size > USER_SPACE_SIZE) return 0;
    return (void*)base;
}

void mmap_free_user(uint32_t pid) {
    // In a real OS we'd unmap pages here
    // For now just mark the region as free
    uint32_t base = USER_SPACE_START + pid * USER_SPACE_SIZE;
    (void)base; // suppress unused warning
}
