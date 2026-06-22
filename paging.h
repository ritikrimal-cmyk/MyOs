#ifndef PAGING_H
#define PAGING_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define PAGE_PRESENT    0x1   // Page is present
#define PAGE_WRITABLE   0x2   // Page is writable
#define PAGE_USER       0x4   // Page is user accessible
#define PAGE_SIZE       4096  // 4KB per page

void paging_init();
void* paging_alloc_page();
void  paging_free_page(void* addr);
void  paging_map(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);

#endif
