#ifndef ELF_H
#define ELF_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

// ELF Magic header
#define ELF_MAGIC 0x464C457F  // 0x7F 'E' 'L' 'F'

// ELF Types
#define ET_EXEC   2   // Executable file
#define PT_LOAD   1   // Loadable segment

struct ElfHeader {
    uint8_t  magic[4];
    uint8_t  bits;        // 1 = 32bit, 2 = 64bit
    uint8_t  endian;      // 1 = little, 2 = big
    uint8_t  version;
    uint8_t  os_abi;
    uint8_t  padding[8];
    uint16_t type;
    uint16_t machine;     // 3 = x86
    uint32_t elf_version;
    uint32_t entry;       // entry point address
    uint32_t ph_offset;   // program header offset
    uint32_t sh_offset;   // section header offset
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_entry_size;
    uint16_t ph_count;    // number of program headers
    uint16_t sh_entry_size;
    uint16_t sh_count;
    uint16_t sh_str_index;
};

struct ProgramHeader {
    uint32_t type;
    uint32_t offset;    // offset in file
    uint32_t vaddr;     // virtual address to load at
    uint32_t paddr;     // physical address
    uint32_t file_size; // size in file
    uint32_t mem_size;  // size in memory
    uint32_t flags;
    uint32_t align;
};

int  elf_validate(uint8_t* data);
uint32_t elf_load(uint8_t* data);

#endif
