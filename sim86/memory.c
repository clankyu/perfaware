#include "memory.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void load_memory_from_file(char *file_name, Seg_Mem *memory) {
    int32_t file_size = 0;

    FILE *file = fopen(file_name, "rb");
    if (!file) {
        printf("Failed to open file\n");
        return;
    }

    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    fread(memory->memory, file_size, 1, file);
    memory->size = file_size;
    free(file);
}

uint8_t *access_memory(Seg_Mem *seg_mem, uint32_t offset) {
    if (offset > seg_mem->size - 1) {
        printf("[ERROR]: accessing memory out of bounds in offset %i\n", offset);
        exit(-1);
    }

     return seg_mem->memory + seg_mem->base + offset;
}

Seg_Mem memory_alloc(Seg_Mem *main_memory, size_t size, uint32_t base) {
    Seg_Mem mem;

    if (base > main_memory->size || base + size > main_memory->size) {
        int32_t end = base + size;
        printf("[ERROR]: allocating memory outside of bounds, start %i, end %i\n", base, end);
        exit(-1);
    }

    mem.memory = main_memory->memory + base;
    mem.base = base;
    mem.size = size;
    mem.mask = 0xffff;

    return mem;
}


void write_u8(uint8_t *dest, uint8_t val) {
    dest[0] = val;
}

void write_u16(uint8_t *dest, uint16_t val) {
    dest[0] = val & 0xFF;
    dest[1] = val >> 8;
}

uint8_t read_u8(uint8_t *dest) {
    return dest[0];
}

uint16_t read_u16(uint8_t *dest) {
    return (uint16_t)dest[0] | ((uint16_t)dest[1] << 8);
}
