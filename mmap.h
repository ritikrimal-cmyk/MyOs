#ifndef MMAP_H
#define MMAP_H

typedef unsigned int uint32_t;

#define USER_SPACE_START  0x01000000  // 16MB — user space starts here
#define USER_SPACE_SIZE   0x01000000  // 16MB per process
#define USER_STACK_TOP    0x02000000  // user stack top

void  mmap_init();
void* mmap_alloc_user(uint32_t pid, uint32_t size);
void  mmap_free_user(uint32_t pid);

#endif
