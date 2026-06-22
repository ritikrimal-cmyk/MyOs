
#include "memory.h"

// Bitmap: 1 bit per 4KB block
// HEAP_SIZE / BLOCK_SIZE = 1024 blocks = 128 bytes of bitmap
#define BLOCK_COUNT (HEAP_SIZE / BLOCK_SIZE)

static uint8_t bitmap[BLOCK_COUNT / 8];

// Set a block as used
static void bitmap_set(int index) {
    bitmap[index / 8] |= (1 << (index % 8));
}

// Set a block as free
static void bitmap_clear(int index) {
    bitmap[index / 8] &= ~(1 << (index % 8));
}

// Check if a block is used
static int bitmap_test(int index) {
    return bitmap[index / 8] & (1 << (index % 8));
}

void memory_init() {
    // Clear the bitmap — all blocks free
    for (int i = 0; i < (int)(BLOCK_COUNT / 8); i++)
        bitmap[i] = 0;
}

// Find first free contiguous 'count' blocks
static int find_free_blocks(int count) {
    int start = -1;
    int found = 0;
    for (int i = 0; i < (int)BLOCK_COUNT; i++) {
        if (!bitmap_test(i)) {
            if (start == -1) start = i;
            found++;
            if (found == count) return start;
        } else {
            start = -1;
            found = 0;
        }
    }
    return -1; // not enough memory
}

void* kmalloc(size_t size) {
    if (size == 0) return 0;

    // How many blocks do we need?
    int blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    int start = find_free_blocks(blocks);
    if (start == -1) return 0; // out of memory

    // Mark blocks as used
    for (int i = start; i < start + blocks; i++)
        bitmap_set(i);

    return (void*)(HEAP_START + start * BLOCK_SIZE);
}

void kfree(void* ptr) {
    if (!ptr) return;

    // Find which block this pointer is in
    int index = ((uint32_t)ptr - HEAP_START) / BLOCK_SIZE;
    if (index < 0 || index >= (int)BLOCK_COUNT) return;

    bitmap_clear(index);
}
