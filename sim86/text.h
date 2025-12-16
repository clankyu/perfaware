#pragma once

#include "instruction.h"

static char *op_type_table[Op_Count];
static char *reg_field_table[2][8];
static char *effective_address_table[3][8];
static char *cond_jump_table_str[16];
static char *label_table[8];

typedef struct {
    char mnemonic[32];
    char dest[32];
    char source[32];
} Instruction_String_Expression;

Instruction_String_Expression get_instruction_str(Instruction *instruction);
void instruction_print(Instruction_String_Expression *expression, Instruction instruction);

void format_displacement(char *source, int16_t displacement);

static char const *get_op_type_str(Operation_Type type);
void get_operand_str(char *dest, Instruction_Operand operand);
void get_register_str(char *dest, Reg_Access reg);
void get_effective_address_str(char *dest, Instruction_Operand operand);
void get_immediate_str(char *dest, Instruction_Operand operand);
void get_none_str(char *dest);

static void print_operand_state(Operand_State state);


