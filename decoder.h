#include <cstdint>

extern uint8_t buffer[];

typedef struct {
    char mnemonic[32];
    char dest[32];
    char source[32];
    uint8_t size;
} Instruction;

typedef struct {
    uint8_t d, w;
    uint8_t mod, reg, rm;
} ModRM;


