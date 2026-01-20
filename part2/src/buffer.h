#pragma once

#include "util.h"

typedef struct {
    u8 *data;
    u32 count;
} Buffer;

b32 in_bounds(Buffer buffer, u32 index);
b32 buffers_are_equal(Buffer a, Buffer b);
Buffer allocate_buffer(u32 count);
void free_buffer(Buffer *buffer);
Buffer buffer_from_file(const char *file_name);
