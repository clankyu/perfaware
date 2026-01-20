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
void write_memory_to_file(char *file_name, void *data, uint32_t n);
uint8_t *access_memory(Seg_Mem *seg_mem, uint32_t offset);
Seg_Mem memory_alloc(Seg_Mem *main_memory, size_t size, uint32_t base);

void write_u8(uint8_t *dest, uint8_t val);
void write_u16(uint8_t *dest, uint16_t val);
uint8_t read_u8(uint8_t *dest);
uint16_t read_u16(uint8_t *dest);
