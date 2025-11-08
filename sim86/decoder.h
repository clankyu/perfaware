#pragma once

#include <stdint.h>

#define MOV_INDEX 0b10001000
#define MOV_MASK 0b11111100
#define IMMD_OP_INDEX 0b10110000
#define IMMD_OP_MASK 0b11110000

#define FN_PARAMS uint8_t *buffer, uint16_t index
extern uint8_t buffer[];

extern char *reg_field_table[2][8];
extern char *effective_address_table[3][8];

typedef struct {
    char mnemonic[32];
    char dest[32];
    char source[32];
    uint8_t size;
} Instruction;

typedef struct {
    uint16_t mask;
    uint16_t opcode;
    Instruction (*decode_fn)(uint8_t*, uint16_t);
} Opcode_Pattern;

extern Opcode_Pattern opcode_patterns[2];

void instruction_print(Instruction *instruction);

typedef struct {
    uint8_t d, w;
    uint8_t mod, reg, rm, padding8;
    uint16_t padding16;
} Instruction_Params;

Instruction decode_instruction(FN_PARAMS);
Instruction default_instruction(FN_PARAMS);
Instruction decode_mov(FN_PARAMS);
Instruction decode_immediate_mov(FN_PARAMS);
Instruction decode_immediate_arithmetic(FN_PARAMS);

void format_displacement(char *dest, char *source, int16_t displacement);


