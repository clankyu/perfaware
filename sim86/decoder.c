#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "decoder.h"

Opcode_Pattern opcode_patterns[29] = {
    { MOV_MASK, MOV_OPCODE, decode_mov },
    { IMMD_OP_MASK, IMMD_OP_OPCODE, decode_immediate_mov },
    { IMMD_OP_ARITHMETIC_MASK, IMMD_OP_ARITHMETIC_OPCODE, decode_immediate_arithmetic },
    { ADD_MASK, ADD_OPCODE, decode_arithmetic },
    { SUB_MASK, SUB_OPCODE, decode_arithmetic },
    { CMP_MASK, CMP_OPCODE, decode_arithmetic },
    { IMMD_TO_ACUMM_MASK, IMMD_TO_ACUMM_ADD_OPCODE, decode_immediate_acumm },
    { IMMD_TO_ACUMM_MASK, IMMD_TO_ACUMM_SUB_OPCODE, decode_immediate_acumm },
    { IMMD_TO_ACUMM_MASK, IMMD_TO_ACUMM_CMP_OPCODE, decode_immediate_acumm },
    { COND_JUMP_MASK, JO_OPCODE,  decode_cond_jump },
    { COND_JUMP_MASK, JNO_OPCODE, decode_cond_jump },
    { COND_JUMP_MASK, JB_OPCODE,  decode_cond_jump },
    { COND_JUMP_MASK, JAE_OPCODE, decode_cond_jump },
    { COND_JUMP_MASK, JE_OPCODE,  decode_cond_jump },
    { COND_JUMP_MASK, JNE_OPCODE, decode_cond_jump },
    { COND_JUMP_MASK, JBE_OPCODE, decode_cond_jump },
    { COND_JUMP_MASK, JA_OPCODE,  decode_cond_jump },
    { COND_JUMP_MASK, JS_OPCODE,  decode_cond_jump },
    { COND_JUMP_MASK, JNS_OPCODE, decode_cond_jump },
    { COND_JUMP_MASK, JP_OPCODE,  decode_cond_jump },
    { COND_JUMP_MASK, JNP_OPCODE, decode_cond_jump },
    { COND_JUMP_MASK, JL_OPCODE,  decode_cond_jump },
    { COND_JUMP_MASK, JNL_OPCODE, decode_cond_jump },
    { COND_JUMP_MASK, JLE_OPCODE, decode_cond_jump },
    { COND_JUMP_MASK, JG_OPCODE,  decode_cond_jump },
    { LOOP_MASK, LOOP_OPCODE,  decode_control_transfer },
    { LOOPZ_MASK, LOOPZ_OPCODE,  decode_control_transfer },
    { LOOPNZ_MASK, LOOPNZ_OPCODE,  decode_control_transfer },
    { JCXZ_MASK, JCXZ_OPCODE,  decode_control_transfer },
};

char *reg_field_table[2][8] = {
    { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" },
    { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" }
};

char *effective_address_table[3][8] = {
    { "[bx + si]", "[bx + di]", "[bp + si]", "[bp + di]",
      "[si]", "[di]", "[DIRECT ADDRESS]", "[bx]" },

    { "[bx + si + ", "[bx + di + ", "[bp + si + ", "[bp + di + ",
      "[si ", "[di + ", "[bp + ", "[bx + " },

    { "[bx + si + ", "[bx + di + ", "[bp + si + ", "[bp + di + ",
      "[si + ", "[di + ", "[bp + ", "[bx + " }
};

char *cond_jump_table[16] = {
    "jo", "jno", "jb", "jnb", "je", "jne", "jbe", "jnbe",
    "js", "jns", "jp", "jnp", "jl", "jnl", "jle", "jnle"
};

char *label_table[8] = {
    "label0", "label1", "label2", "label3",
    "label4", "label5", "label6", "label7"
};

void instruction_print(Instruction *instruction) {
    printf("%s %s, %s\n", instruction->mnemonic, instruction->dest, instruction->source);
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

void format_displacement(char *dest, char *source, int16_t displacement) {
    if (displacement == 0) {
        for (int i = 0; i < 3; i++) {
            dest[i] = source[i];
        }
        dest[3] = ']';
        dest[4] = '\0';
    } else {
        char append[32];
        sprintf(append, "%d]", displacement);
        strcat(dest, append);
    }
}

Instruction decode_instruction(uint8_t *buffer, uint16_t index) {
    uint8_t opcode = buffer[index];
    printf("opcode: %i\n", opcode);
    for (int bit = 7; bit >= 0; bit--) {
        printf("%d", (opcode >> bit) & 1);
    }
    printf("\n");
    Instruction inst;
    return inst;
}

Instruction default_instruction(FN_PARAMS) {
    char source[32];
    sprintf(source, "index: %i", index);

    Instruction instr = (Instruction) {
        .mnemonic = "default",
        .dest = "unimplemented opcode",
    };
    strcpy(instr.source, source);

    return instr;
}

Instruction decode_mov(FN_PARAMS) {
    Instruction instr;
    Instruction_Params params;

    uint8_t byte1 = buffer[index];
    uint8_t byte2 = buffer[index + 1];

    params.d = (byte1 >> 1) & 0b1;
    params.w = byte1 & 0b1;
    params.mod = (byte2 >> 6) & 0b11;
    params.reg = (byte2 >> 3) & 0b111;
    params.rm = byte2 & 0b111;

    strcpy(instr.mnemonic, "mov");
    strcpy(instr.dest, reg_field_table[params.w][params.reg]);

    if (params.mod == 0) {
        // exception here according to the manual for rm = 0b110
        if (params.rm == 0b110) {
            int16_t displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
            sprintf(instr.source, "[%d]", displacement);
            instr.size = 4;
        } else {
            strcpy(instr.source, effective_address_table[params.mod][params.rm]);
            instr.size = 2;
        }
    } else if (params.mod == 0b01 || params.mod == 0b10) {
        strcpy(instr.source, effective_address_table[params.mod][params.rm]);

        int16_t displacement = 0;
        if (params.mod == 0b01) {
            displacement = (int8_t) buffer[index+2];
            instr.size = 3;
        } else if (params.mod == 0b10) {
            displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
            instr.size = 4;
        }

        format_displacement(instr.source, effective_address_table[params.mod][params.rm], displacement);
    } else if (params.mod == 0b11) {
        strcpy(instr.source, reg_field_table[params.w][params.rm]);
        instr.size = 2;
    }

    if (params.d == 0) {
        char temp[32];
        strcpy(temp, instr.source);
        strcpy(instr.source, instr.dest);
        strcpy(instr.dest, temp);
    }

    return instr;
}

Instruction decode_immediate_mov(FN_PARAMS) {
    Instruction instr;

    uint8_t byte = buffer[index];
    uint8_t w = (byte >> 3) & 0b1;
    uint8_t reg = byte & 0b111;

    strcpy(instr.mnemonic, "mov");
    strcpy(instr.dest, reg_field_table[w][reg]);

    if (w) {
        int16_t s = (int16_t)buffer[index+1] | ((int16_t)buffer[index+2] << 8);
        sprintf(instr.source, "%d", s);
        instr.size = 3;
    } else {
        sprintf(instr.source, "%d", (int8_t) buffer[index+1]);
        instr.size = 2;
    }

    return instr;
}

Instruction decode_immediate_arithmetic(FN_PARAMS) {
    Instruction instr;

    uint8_t byte = buffer[index];
    uint8_t s = (byte >> 1) & 0b1;
    uint8_t w = byte & 0b1;
    uint8_t mod = (buffer[index+1] >> 6) & 0b11;
    uint8_t rm = buffer[index+1] & 0b111;
    instr.size = 2;

    int16_t displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
    int16_t data = 0;

    uint8_t instruction_type = (buffer[index+1] >> 3) & 0b111;
    switch (instruction_type) {
        case 0b000:
            strcpy(instr.mnemonic, "add");
            break;
        case 0b101:
            strcpy(instr.mnemonic, "sub");
            break;
        case 0b111:
            strcpy(instr.mnemonic, "cmp");
            break;
    }

    int16_t data_idx = index + 2;
    switch (mod) {
        case 0b00:
            if (rm == 0b110) {
                sprintf(instr.dest, "[%d]", displacement);
                data_idx = index + 4;
                instr.size += 2;
            } else {
                data_idx = index + 2;
                strcpy(instr.dest, effective_address_table[mod][rm]);
            }
            break;
        case 0b01:
            strcpy(instr.dest, effective_address_table[mod][rm]);
            format_displacement(instr.dest, effective_address_table[mod][rm], displacement);
            instr.size++;
            data_idx = index + 3;
            break;
        case 0b10:
            strcpy(instr.dest, effective_address_table[mod][rm]);
            format_displacement(instr.dest, effective_address_table[mod][rm], displacement);
            instr.size += 2;
            data_idx = index + 4;
            break;
        case 0b11:
            strcpy(instr.dest, reg_field_table[w][rm]);
            data_idx = index + 2;
            break;
        default:
            printf("[ERROR]: decode_immediate_arithmetic, mod is not 0b00, 0b01, 0b10 or 0b11, mod: %i\n", mod);
    }

    if (w) {
        if (s) {
            data = (int16_t)(int8_t)buffer[data_idx];
            instr.size++;
        } else {
            data = (int16_t)buffer[data_idx] | ((int16_t)buffer[data_idx+1] << 8);
            instr.size += 2;
        }
    } else {
        data = buffer[data_idx];
        instr.size++;
    }

    sprintf(instr.source, "%d", data);

    return instr;
}

Instruction decode_arithmetic(FN_PARAMS) {
    Instruction instr;
    Instruction_Params params;

    uint8_t byte1 = buffer[index];
    uint8_t byte2 = buffer[index + 1];

    params.d = (byte1 >> 1) & 0b1;
    params.w = byte1 & 0b1;
    params.mod = (byte2 >> 6) & 0b11;
    params.reg = (byte2 >> 3) & 0b111;
    params.rm = byte2 & 0b111;

    switch (byte1 & ARITHMETIC_MASK) {
        case ADD_OPCODE:
            strcpy(instr.mnemonic, "add");
            break;
        case SUB_OPCODE:
            strcpy(instr.mnemonic, "sub");
            break;
        case CMP_OPCODE:
            strcpy(instr.mnemonic, "cmp");
            break;
    }

    strcpy(instr.dest, reg_field_table[params.w][params.reg]);

    if (params.mod == 0) {
        // exception here according to the manual for rm = 0b110
        if (params.rm == 0b110) {
            int16_t displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
            sprintf(instr.source, "[%d]", displacement);
            instr.size = 4;
        } else {
            strcpy(instr.source, effective_address_table[params.mod][params.rm]);
            instr.size = 2;
        }
    } else if (params.mod == 0b01 || params.mod == 0b10) {
        strcpy(instr.source, effective_address_table[params.mod][params.rm]);

        int16_t displacement = 0;
        if (params.mod == 0b01) {
            displacement = (int8_t) buffer[index+2];
            instr.size = 3;
        } else if (params.mod == 0b10) {
            displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
            instr.size = 4;
        }

        format_displacement(instr.source, effective_address_table[params.mod][params.rm], displacement);
    } else if (params.mod == 0b11) {
        strcpy(instr.source, reg_field_table[params.w][params.rm]);
        instr.size = 2;
    }

    if (params.d == 0) {
        char temp[32];
        strcpy(temp, instr.source);
        strcpy(instr.source, instr.dest);
        strcpy(instr.dest, temp);
    }

    return instr;
}

Instruction decode_immediate_acumm(FN_PARAMS) {
    Instruction instr;
    uint8_t byte = buffer[index];
    uint8_t w = byte & 0b1;
    int16_t data = 0;

    strcpy(instr.dest, reg_field_table[w][0]);

    uint8_t opcode = byte & IMMD_TO_ACUMM_MASK;
    if (opcode == IMMD_TO_ACUMM_ADD_OPCODE) {
        strcpy(instr.mnemonic, "add");
    } else if (opcode == IMMD_TO_ACUMM_SUB_OPCODE) {
        strcpy(instr.mnemonic, "sub");
    } else if (opcode == IMMD_TO_ACUMM_CMP_OPCODE) {
        strcpy(instr.mnemonic, "cmp");
    }

    if (w) {
        data = (int16_t)buffer[index+1] | ((int16_t)buffer[index+2] << 8);
        instr.size = 3;
    } else {
        data = (int8_t)buffer[index+1];
        instr.size = 2;
    }

    sprintf(instr.source, "%d", data);

    return instr;
}

Instruction decode_cond_jump(FN_PARAMS) {
    Instruction instr;
    uint8_t byte = buffer[index];
    int8_t offset = (int8_t) buffer[index + 1];
    strcpy(instr.mnemonic, cond_jump_table[COND_JUMP_INDEX(byte)]);
    strcpy(instr.source, "\0");
    sprintf(instr.dest, "%i", offset);

    instr.size = 2;

    return instr;
}

Instruction decode_control_transfer(FN_PARAMS) {
    Instruction instr;
    uint8_t byte = buffer[index];
    int8_t offset = buffer[index+1];
    strcpy(instr.source, "\0");
    if (byte == LOOP_OPCODE) {
        strcpy(instr.mnemonic, "loop");
    } else if (byte == LOOPZ_OPCODE) {
        strcpy(instr.mnemonic, "loopz");
    } else if (byte == LOOPNZ_OPCODE) {
        strcpy(instr.mnemonic, "loopnz");
    } else if (byte == JCXZ_OPCODE) {
        strcpy(instr.mnemonic, "jcxz");
    }

    sprintf(instr.dest, "%i", offset);
    instr.size = 2;

    return instr;
}
