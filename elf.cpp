#include "elf.h"
#include "memory.h"
#include "vga.h"

static void mem_copy(uint8_t* dst, uint8_t* src, uint32_t size) {
    for (uint32_t i = 0; i < size; i++)
        dst[i] = src[i];
}

static void mem_zero(uint8_t* dst, uint32_t size) {
    for (uint32_t i = 0; i < size; i++)
        dst[i] = 0;
}

int elf_validate(uint8_t* data) {
    ElfHeader* hdr = (ElfHeader*)data;

    // Check magic number
    if (hdr->magic[0] != 0x7F ||
        hdr->magic[1] != 'E'  ||
        hdr->magic[2] != 'L'  ||
        hdr->magic[3] != 'F') {
        vga_println("ELF: Invalid magic number", COLOR_LIGHT_RED);
        return -1;
    }

    // Check 32-bit
    if (hdr->bits != 1) {
        vga_println("ELF: Not a 32-bit binary", COLOR_LIGHT_RED);
        return -1;
    }

    // Check executable
    if (hdr->type != ET_EXEC) {
        vga_println("ELF: Not an executable", COLOR_LIGHT_RED);
        return -1;
    }

    // Check x86
    if (hdr->machine != 3) {
        vga_println("ELF: Not an x86 binary", COLOR_LIGHT_RED);
        return -1;
    }

    return 0;
}

uint32_t elf_load(uint8_t* data) {
    ElfHeader* hdr = (ElfHeader*)data;

    if (elf_validate(data) != 0)
        return 0;

    // Load each program segment
    for (uint16_t i = 0; i < hdr->ph_count; i++) {
        ProgramHeader* ph = (ProgramHeader*)(data + hdr->ph_offset + i * hdr->ph_entry_size);

        if (ph->type != PT_LOAD) continue;

        // Copy segment to virtual address
        uint8_t* dst = (uint8_t*)ph->vaddr;
        uint8_t* src = data + ph->offset;

        mem_copy(dst, src, ph->file_size);

        // Zero out remaining memory (BSS)
        if (ph->mem_size > ph->file_size)
            mem_zero(dst + ph->file_size, ph->mem_size - ph->file_size);
    }

    // Return entry point
    return hdr->entry;
}
