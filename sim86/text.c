#include <stdio.h>
#include <string.h>
#include "text.h"
#include "execute.h"
#include "instruction.h"

static char *op_type_table[Op_Count] = {
    #define OP(name) #name,
    #include "op_list.def"
    #undef OP
};

static char *reg_field_table[2][8] = {
    { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" },
    { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" }
};

static char *effective_address_table[3][8] = {
    { "[bx + si]", "[bx + di]", "[bp + si]", "[bp + di]",
      "[si]", "[di]", "[DIRECT ADDRESS]", "[bx]" },

    { "[bx + si + ", "[bx + di + ", "[bp + si + ", "[bp + di + ",
      "[si ", "[di + ", "[bp + ", "[bx + " },

    { "[bx + si + ", "[bx + di + ", "[bp + si + ", "[bp + di + ",
      "[si + ", "[di + ", "[bp + ", "[bx + " }
};

static char *cond_jump_table_str[16] = {
    "jo", "jno", "jb", "jnb", "je", "jne", "jbe", "jne",
    "js", "jns", "jp", "jnp", "jl", "jnl", "jle", "ja"
};

static char *segment_register_table[4] = {
    "es", "cs", "ss", "ds"
};

Instruction_String_Expression get_instruction_str(Instruction *instruction) {
    Instruction_String_Expression expr = {};
    strcpy(expr.mnemonic, get_op_type_str(instruction->op_type));
    get_operand_str(expr.dest, instruction->dest);
    get_operand_str(expr.source, instruction->source);

    return expr;
}

void instruction_print(Instruction instruction, Instruction_String_Expression *expression) {
    if (instruction.op_type >= Op_je && instruction.op_type <= Op_jcxz) {
        printf("%s .L%u\n", expression->mnemonic, instruction.address + instruction.size + (int8_t)(instruction.dest.immediate.val & 0xFF));
    } else if (strcmp(expression->source, "\0") == 0) {
        printf("%s %s\n", expression->mnemonic, expression->dest);
    } else {
        printf("%s %s, %s\n", expression->mnemonic, expression->dest, expression->source);
    }
}

void get_flags_str(char *dest, uint16_t flags) {
    char *p = dest;

    if (flags & FLAG_OF) *p++ = 'O';
    if (flags & FLAG_DF) *p++ = 'D';
    if (flags & FLAG_IF) *p++ = 'I';
    if (flags & FLAG_TF) *p++ = 'T';
    if (flags & FLAG_SF) *p++ = 'S';
    if (flags & FLAG_ZF) *p++ = 'Z';
    if (flags & FLAG_AF) *p++ = 'A';
    if (flags & FLAG_PF) *p++ = 'P';
    if (flags & FLAG_CF) *p++ = 'C';

    *p = '\0';
}

void format_displacement(char *source, int16_t displacement) {
    snprintf(source, 32, "%s%d]", source, displacement);
}

char const *get_op_type_str(Operation_Type type) {
    return op_type_table[type];
}

void get_operand_str(char *dest, Instruction_Operand operand) {
    switch (operand.type) {
        case Operand_Register:
            get_register_str(dest, operand);
            return;
        case Operand_Memory:
            get_effective_address_str(dest, operand);
            return;
        case Operand_Immediate:
            get_immediate_str(dest, operand);
            return;
        case Operand_None:
            get_none_str(dest);
            return;
    }

    return;
}

void get_register_str(char *dest, Instruction_Operand operand) {
    if (operand.reg.type == Register_General) {
        strcpy(dest, reg_field_table[operand.reg.w][operand.reg.reg_rm]);
    } else {
        strcpy(dest, segment_register_table[operand.reg.sr]);
    }
}

void get_effective_address_str(char *dest, Instruction_Operand operand) {
    char casting[6];
    uint16_t address = calculate_effective_address(operand.address);

    if (operand.address.is_word) {
        snprintf(casting, 5, "word");
    } else {
        snprintf(casting, 5, "byte");
    }

    if (operand.address.mod == 0 && operand.address.rm == 0b110) {
        snprintf(dest, 32, "%s [%d]", casting, operand.address.displacement);
    } else if (operand.address.mod == 0) {
        snprintf(dest, 32, "%s %s", casting, effective_address_table[operand.address.mod][operand.address.rm]);
    } else {
        snprintf(dest, 32, "%s %s%d]",
                 casting,
                 effective_address_table[operand.address.mod][operand.address.rm],
                 operand.address.displacement);
    }
}

void get_immediate_str(char *dest, Instruction_Operand operand) {
    if (operand.immediate.is_signed) {
        int16_t signed_val = (int16_t) operand.immediate.val;
        snprintf(dest, 32, "%d", signed_val);
    } else {
        snprintf(dest, 32, "%d", operand.immediate.val);
    }
}

void get_none_str(char *dest) {
    strcpy(dest, "\0");
}

void print_instruction_and_operand_state(Instruction *instruction, Instruction_String_Expression *expression, Operand_State operand_state, Flags_State flags_state) {
    char operand[32];
    get_operand_str(operand, operand_state.operand);

    char flags_before[32];
    char flags_after[32];
    get_flags_str(flags_before, flags_state.before);
    get_flags_str(flags_after, flags_state.after);

    if (instruction->source.type == Operand_None) {
        printf("%s %s\n", expression->mnemonic, expression->dest);
    } else {
        printf("%s %s, %s { %s : 0x%X -> 0x%X | flags : %s -> %s }\n",
               expression->mnemonic, expression->dest, expression->source, operand, operand_state.before, operand_state.after,
               flags_before, flags_after);
    }
}

void print_bits(uint8_t *data, uint32_t size) {
    for (size_t i = 0; i < size; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            printf("%d", (data[i] >> bit) & 1);
        }
        printf(" ");
    }
}

