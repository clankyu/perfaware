#include <stdint.h>

#define MAX_MEMORY_SIZE (1024*1024)
typedef struct {
    uint8_t bytes[MAX_MEMORY_SIZE];
    uint32_t offset;
} Memory;
#define MEMORY_ACCESS_MASK 0xFFFFF

void load_memory_from_file(char *file_name, Memory *memory);
void *memory_alloc(Memory *memory, int32_t size);
