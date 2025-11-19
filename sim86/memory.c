#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

void load_memory_from_file(char *file_name, Memory *memory) {
    int32_t file_size = 0;

    FILE *file = fopen(file_name, "rb");
    if (!file) {
        printf("Failed to open file\n");
        return;
    }

    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    fread(memory->bytes, file_size, 1, file);
    memory->offset = file_size;
    free(file);
}

void *memory_alloc(Memory *memory, int32_t size) {
    int32_t final_size = memory->offset + size;
    if (final_size > MAX_MEMORY_SIZE) {
        printf("[ERROR]: reached maximum memory size, exiting program\n");
        exit(-1);
    }

    void *ptr = &memory->bytes[memory->offset + 1];
    memory->offset += size;
    return ptr;
}
