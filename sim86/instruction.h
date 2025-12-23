#pragma once

#include "memory.h"
#include <stdbool.h>

typedef enum operation_type {
    #define OP(name) Op_##name,
    #include "op_list.def"
    #undef OP
    Op_Count, // insane trick oml it's literally the count of enum variants awesome sauce
} Operation_Type;

typedef struct {
    Seg_Mem *mem;
    uint32_t val;
} Operand_Access;

typedef enum {
    Operand_None,
    Operand_Register,
    Operand_Memory,
    Operand_Immediate
} Operand_Type;

typedef struct {
    uint8_t mod;
    uint8_t rm;
    int16_t displacement;
    bool is_word;
} Effective_Address_Expression;

typedef struct {
    bool exists;
    uint8_t *reg;
} Effective_Address_Operand;

typedef struct {
    int32_t val;
} Immediate;

typedef struct {
    uint16_t w_mod;
    uint16_t reg_rm;
} Reg_Access;

typedef struct {
    Operand_Type type;
    union {
        Effective_Address_Expression address;
        Immediate immediate;
        Reg_Access reg;
    };
} Instruction_Operand;

typedef struct {
    Instruction_Operand operand;
    uint16_t before;
    uint16_t after;
} Operand_State;

typedef struct {
    uint16_t address;

    uint8_t size;
    Operation_Type op_type;

    Instruction_Operand dest;
    Instruction_Operand source;
} Instruction;

typedef struct {
    uint8_t d, w;
    uint8_t mod, reg, rm, padding8;
    uint16_t padding16;
} Instruction_Params;

Instruction_Operand register_operand(Instruction_Params params, bool mod_rm);
Instruction_Operand effective_address_operand(Instruction_Params params, uint16_t displacement);
Instruction_Operand immediate_operand(int16_t value);
Instruction_Operand none_operand();

static Operand_State save_operand_state(Instruction_Operand operand, int16_t before, int16_t after);

