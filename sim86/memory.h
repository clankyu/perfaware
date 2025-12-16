#pragma once

#include <stddef.h>
#include <stdint.h>

#define PROGRAM_MEM_SIZE 0xFFFF

typedef struct {
    uint8_t *memory;
    uint16_t base;
    uint16_t size;
    uint16_t mask;
} Seg_Mem;

void load_memory_from_file(char *file_name, Seg_Mem *memory);
uint8_t *access_memory(Seg_Mem *seg_mem, uint32_t offset);
Seg_Mem memory_alloc(Seg_Mem *main_memory, size_t size, uint32_t base);
