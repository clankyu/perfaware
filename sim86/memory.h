#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *memory;
    uint16_t base;
    uint16_t offset;
    uint16_t mask;

} Seg_Mem;

void load_memory_from_file(char *file_name, Seg_Mem *memory);
void *memory_alloc(size_t size);
