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

static char *label_table[8] = {
    "label0", "label1", "label2", "label3",
    "label4", "label5", "label6", "label7"
};

Instruction_String_Expression get_instruction_str(Instruction *instruction) {
    Instruction_String_Expression expr = {};
    strcpy(expr.mnemonic, get_op_type_str(instruction->op_type));
    get_operand_str(expr.dest, instruction->dest);
    get_operand_str(expr.source, instruction->source);

    return expr;
}

void instruction_print(Instruction_String_Expression *expression, Instruction instruction) {
    if (instruction.source.type == Operand_None) {
        printf("%s %s\n", expression->mnemonic, expression->dest);
    } else {
        printf("%s %s, %s\n", expression->mnemonic, expression->dest, expression->source);
    }
}

void format_displacement(char *source, int16_t displacement) {
    snprintf(source, 32, "%s%d]", source, displacement);
}

static char const *get_op_type_str(Operation_Type type) {
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
    strcpy(dest, reg_field_table[operand.reg.w_mod][operand.reg.reg_rm]);
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
    snprintf(dest, 32, "%d", operand.immediate.val);
}

void get_none_str(char *dest) {
    strcpy(dest, "\0");
}

void print_instruction_and_operand_state(Instruction *instruction, Instruction_String_Expression *expression, Operand_State state) {
    char operand[32];
    get_operand_str(operand, state.operand);

    //printf("%s : %d -> %d\n", operand, state.before, state.after);
    if (instruction->source.type == Operand_None) {
        printf("%s %s\n", expression->mnemonic, expression->dest);
    } else {
        printf("%s %s, %s { %s : %d -> %d }\n", expression->mnemonic, expression->dest, expression->source, operand, state.before, state.after);
    }
}

void print_bits(uint8_t *data, uint32_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("bit %i: ", (int) i);
        for (int bit = 7; bit >= 0; bit--) {
            printf("%d", (data[i] >> bit) & 1);
        }
        printf(" ");
    }
}

