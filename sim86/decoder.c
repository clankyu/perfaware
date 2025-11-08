#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "decoder.h"

Opcode_Pattern opcode_patterns[2] = {
    { MOV_MASK, MOV_INDEX, decode_mov },
    { IMMD_OP_MASK, IMMD_OP_INDEX, decode_immediate_mov }
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

void instruction_print(Instruction *instruction) {
    printf("%s %s, %s\n", instruction->mnemonic, instruction->dest, instruction->source);
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

Instruction default_instruction(uint8_t *buffer, uint16_t index) {
    char source[32];
    sprintf(source, "index: %i", index);

    Instruction instr = (Instruction) {
        .mnemonic = "default",
        .dest = "unimplemented opcode",
    };
    strcpy(instr.source, source);

    return instr;
}

Instruction decode_mov(uint8_t *buffer, uint16_t index) {
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

Instruction decode_immediate_mov(uint8_t *buffer, uint16_t index) {
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

Instruction decode_immediate_arithmetic(uint8_t *buffer, uint16_t index) {
    Instruction instr;

    uint8_t byte = buffer[index];
    uint8_t s = (byte >> 1) & 0b1;
    uint8_t w = byte & 0b1;
    uint8_t mod = (buffer[index+1] >> 6) & 0b11;
    uint8_t rm = buffer[index+1] & 0b111;

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

    if (w) {
        if (s) {
            data = (int16_t)(int8_t) buffer[index+4];
        } else {
            data = (int16_t)buffer[index+4] | ((int16_t)buffer[index+5] << 8);
        }
    } else {
        data = (int8_t) buffer[index+4];
    }

    sprintf(instr.source, "%d", data);

    switch (mod) {
        case 0b00:
            if (rm == 0b110) {
                sprintf(instr.dest, "[%d]", displacement);
            } else {
                strcpy(instr.dest, effective_address_table[mod][rm]);
            }
            break;
        case 0b01:
            format_displacement(instr.dest, effective_address_table[mod][rm], displacement);
            break;
        case 0b10:
            format_displacement(instr.dest, effective_address_table[mod][rm], displacement);
            break;
        case 0b11:
            strcpy(instr.dest, reg_field_table[w][rm]);
            break;
        default:
            printf("[ERROR]: decode_immediate_arithmetic, mod is not 0b00, 0b01, 0b10 or 0b11, mod: %i\n", mod);
    }

    return instr;
}
