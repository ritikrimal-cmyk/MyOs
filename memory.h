#ifndef MEMORY_H
#define MEMORY_H

// Define our own types — no system headers needed
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned int       size_t;

#define HEAP_START  0x00400000
#define HEAP_SIZE   0x00400000
#define BLOCK_SIZE  4096

void memory_init();
void* kmalloc(size_t size);
void kfree(void* ptr);

#endif
