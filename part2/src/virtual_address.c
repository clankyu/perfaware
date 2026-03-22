#include <stdio.h>
#include <stdlib.h>
#include "util.h"

typedef struct {
    u16 pml4_index;
    u16 directory_ptr_index;
    u16 directory_index;
    u16 table_index;
    u32 offset;
} Decomposed_Virtual_Address;

Decomposed_Virtual_Address decompose_pointer_4kb(void *ptr);
Decomposed_Virtual_Address decompose_pointer_2mb(void *ptr);
Decomposed_Virtual_Address decompose_pointer_1gb(void *ptr);
void print_bits(u64 val, u64 count, u64 shr);
void print_decomposed_pointer(Decomposed_Virtual_Address addr);

int main(void) {
    u8 *memory = malloc(1024*1024);

    u64 page_size = 4096;
    u32 page_count = (1024*1024) / page_size;
    for (u32 page_index = 0; page_index < page_count; page_index++) {
        u64 addr = (u64) (memory + (page_index * page_size));
        Decomposed_Virtual_Address addr_4kb = decompose_pointer_4kb(memory + (page_index * 4096));
        Decomposed_Virtual_Address addr_2mb = decompose_pointer_2mb(memory + (page_index * 4096));
        Decomposed_Virtual_Address addr_1gb = decompose_pointer_1gb(memory + (page_index * 4096));

        memory[page_index * page_size] = 1;
        printf("|0000000000000000|");
        print_bits(addr, 9, 39); printf("|");
        print_bits(addr, 9, 30); printf("|");
        print_bits(addr, 9, 21); printf("|");
        print_bits(addr, 9, 12); printf("|");
        print_bits(addr, 12, 0); printf("|\n");
        printf("4kb:    "); print_decomposed_pointer(addr_4kb); printf("\n");
        printf("2mb:    "); print_decomposed_pointer(addr_2mb); printf("\n");
        printf("1gb:    "); print_decomposed_pointer(addr_1gb); printf("\n");
        printf("\n");
    }

    return 0;
}

Decomposed_Virtual_Address decompose_pointer_4kb(void *ptr) {
    Decomposed_Virtual_Address result = {0};
    u64 addr = (u64)ptr;

    result.offset = addr & 0xFFF;
    result.pml4_index = (addr >> (39)) & 0x1FF;
    result.directory_ptr_index = (addr >> (30)) & 0x1FF;
    result.directory_index = (addr >> (21)) & 0x1FF;
    result.table_index = (addr >> (12)) & 0x1FF;

    return result;
}

Decomposed_Virtual_Address decompose_pointer_2mb(void *ptr) {
    Decomposed_Virtual_Address result = {0};
    u64 addr = (u64)ptr;

    result.offset = addr & 0x1FFFFF;
    result.pml4_index = (addr >> (39)) & 0x1FF;
    result.directory_ptr_index = (addr >> (30)) & 0x1FF;
    result.directory_index = (addr >> (21)) & 0x1FF;

    return result;
}

Decomposed_Virtual_Address decompose_pointer_1gb(void *ptr) {
    Decomposed_Virtual_Address result = {0};
    u64 addr = (u64)ptr;

    result.offset = addr & 0x3FFFFFFF;
    result.pml4_index = (addr >> (39)) & 0x1FF;
    result.directory_ptr_index = (addr >> (30)) & 0x1FF;

    return result;
}

void print_decomposed_pointer(Decomposed_Virtual_Address addr) {
    printf("|%3u|%3u|%3u|%3u|%10u|",
           addr.pml4_index, addr.directory_ptr_index, addr.directory_index,
           addr.table_index, addr.offset);
}

void print_bits(u64 val, u64 count, u64 shr) {
    u64 new_val = val >> shr;
    for (u32 bit_index = 0; bit_index < count; ++bit_index) {
        printf("%1lu", (new_val >> (count - bit_index - 1)) & 1);
    }
}
