#include "util.h"
#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"

b32 in_bounds(Buffer buffer, u32 index) {
    if (buffer.count < index) {
        return false;
    }

    return true;
}

b32 buffers_are_equal(Buffer a, Buffer b) {
    if (a.count != b.count) { return false; }
    for (u32 i = 0; i < a.count; i++) {
        if (a.data[i] != b.data[i]) { return false; }
    }

    return true;
}

Buffer allocate_buffer(u32 count) {
    Buffer result = {0};

    result.data = (u8*) malloc(count);
    if (result.data) {
        result.count = count;
    } else {
        result.count = 0;
        fprintf(stderr, "Error allocating data, count: %u\n", count);
    }

    return result;
}

void free_buffer(Buffer *buffer) {
    free(buffer->data);
    buffer->data = NULL;
    *buffer = (Buffer){0};
}

Buffer buffer_from_file(const char *file_name) {
    Buffer result = {0};
    FILE *file = fopen(file_name, "rb");
    if (!file) {
        fprintf(stderr, "Error while opening file %s\n", file_name);
    }

    u32 file_size = 0;
    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    result = allocate_buffer(file_size);
    fread(result.data, result.count, 1, file);

    fclose(file);

    return result;
}

void write_buffer_to_file(char *file_name, Buffer *buffer) {
    FILE *file = fopen(file_name, "wb");
    if (!file) {
        fprintf(stderr, "Error while opening file to write %s\n", file_name);
        return;
    }

    fwrite(buffer->data, 1, buffer->count, file);

    fclose(file);
}
