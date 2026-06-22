#include "paging.h"
#include "memory.h"

typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;

// Page directory — 1024 entries, each points to a page table
static uint32_t page_directory[1024] __attribute__((aligned(4096)));

// One page table for the first 4MB (kernel space)
static uint32_t kernel_page_table[1024] __attribute__((aligned(4096)));

// Physical memory bitmap for page allocation
#define PHYS_MEM_START  0x00800000
#define PHYS_MEM_SIZE   0x01000000
#define PHYS_PAGE_COUNT (PHYS_MEM_SIZE / PAGE_SIZE)

static uint8_t phys_bitmap[PHYS_PAGE_COUNT / 8];

static void phys_set(int idx) {
    phys_bitmap[idx / 8] |= (1 << (idx % 8));
}

static void phys_clear(int idx) {
    phys_bitmap[idx / 8] &= ~(1 << (idx % 8));
}

static int phys_test(int idx) {
    return phys_bitmap[idx / 8] & (1 << (idx % 8));
}

static int phys_find_free() {
    for (int i = 0; i < PHYS_PAGE_COUNT; i++)
        if (!phys_test(i)) return i;
    return -1;
}

void* paging_alloc_page() {
    int idx = phys_find_free();
    if (idx == -1) return 0;
    phys_set(idx);
    return (void*)(PHYS_MEM_START + idx * PAGE_SIZE);
}

void paging_free_page(void* addr) {
    int idx = ((uint32_t)addr - PHYS_MEM_START) / PAGE_SIZE;
    if (idx >= 0 && idx < PHYS_PAGE_COUNT)
        phys_clear(idx);
}

void paging_map(uint32_t virt, uint32_t phys, uint32_t flags) {
    uint32_t pd_idx = virt >> 22;
    uint32_t pt_idx = (virt >> 12) & 0x3FF;

    uint32_t* page_table;
    if (page_directory[pd_idx] & PAGE_PRESENT) {
        page_table = (uint32_t*)(page_directory[pd_idx] & 0xFFFFF000);
    } else {
        page_table = (uint32_t*)paging_alloc_page();
        if (!page_table) return;
        for (int i = 0; i < 1024; i++)
            page_table[i] = 0;
        page_directory[pd_idx] = (uint32_t)page_table | PAGE_PRESENT | PAGE_WRITABLE;
    }

    page_table[pt_idx] = (phys & 0xFFFFF000) | (flags & 0xFFF) | PAGE_PRESENT;
}

void paging_init() {
    // Clear physical bitmap
    for (int i = 0; i < (int)(PHYS_PAGE_COUNT / 8); i++)
        phys_bitmap[i] = 0;

    // Clear page directory
    for (int i = 0; i < 1024; i++)
        page_directory[i] = 0;

    // Identity map first 4MB (kernel code)
    for (int i = 0; i < 1024; i++)
        kernel_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    page_directory[0] = (uint32_t)kernel_page_table | PAGE_PRESENT | PAGE_WRITABLE;

    // Identity map 4MB-8MB for kernel heap
    uint32_t* heap_page_table = (uint32_t*)paging_alloc_page();
    if (heap_page_table) {
        for (int i = 0; i < 1024; i++)
            heap_page_table[i] = (0x400000 + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
        page_directory[1] = (uint32_t)heap_page_table | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // Identity map framebuffer at 0xFD000000 (16MB)
    for (uint32_t fb_addr = 0xFD000000; fb_addr < 0xFE000000; fb_addr += 0x400000) {
        uint32_t* fb_page_table = (uint32_t*)paging_alloc_page();
        if (fb_page_table) {
            for (int i = 0; i < 1024; i++)
                fb_page_table[i] = (fb_addr + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
            uint32_t pd_idx = fb_addr >> 22;
            page_directory[pd_idx] = (uint32_t)fb_page_table | PAGE_PRESENT | PAGE_WRITABLE;
        }
    }

    // Load page directory into CR3
    asm volatile("mov %0, %%cr3" :: "r"((uint32_t)page_directory));

    // Enable paging
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}
