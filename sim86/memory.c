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
    memory->offset = file_size;
    free(file);
}

