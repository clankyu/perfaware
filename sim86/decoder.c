#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "decoder.h"
#include "instruction.h"

Opcode_Pattern opcode_patterns[31] = {
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
    { COND_JUMP_MASK, JNB_OPCODE, decode_cond_jump },
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
    { MOV_REGMEM_TO_SEG_MASK, MOV_REGMEM_TO_SEG_OPCODE,  decode_segment_register_mov },
    { MOV_SEG_TO_REGMEM_MASK, MOV_SEG_TO_REGMEM_OPCODE,  decode_segment_register_mov },
};

static uint32_t cond_jump_table[16] = {
    Op_jo, Op_jno, Op_jb, Op_jnb, Op_je, Op_jne, Op_jbe, Op_jne,
    Op_js, Op_jns, Op_jp, Op_jnp, Op_jl, Op_jnl, Op_jle, Op_ja
};

Operation_Type get_cond_jump_type(uint8_t opcode) {
    return cond_jump_table[COND_JUMP_INDEX(opcode)];
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
        .op_type = Op_Unimplemented,
        .size = 1,
        .address = index,
        .dest = (Instruction_Operand) {
            .type = Operand_Immediate,
            .immediate = 6761,
        },
        .source = (Instruction_Operand) {
            .type = Operand_Immediate,
            .immediate = 6761
        }
    };

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

    instr.op_type = Op_mov;
    instr.dest = general_register_operand(params, false);

    if (params.mod == 0) {
        // exception here according to the manual for rm = 0b110
        if (params.rm == 0b110) {
            int16_t displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
            instr.source = effective_address_operand(params, displacement);
            instr.size = 4;
            instr.source.address.is_word = params.w;
        } else {
            instr.source = effective_address_operand(params, 0);
            instr.size = 2;
        }
    } else if (params.mod == 0b01 || params.mod == 0b10) {
        int16_t displacement = 0;
        if (params.mod == 0b01) {
            displacement = (int8_t) buffer[index+2];
            instr.source = effective_address_operand(params, displacement);
            instr.size = 3;
            instr.source.address.is_word = params.w;
        } else if (params.mod == 0b10) {
            displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
            instr.source = effective_address_operand(params, displacement);
            instr.size = 4;
            instr.source.address.is_word = params.w;
        }
    } else if (params.mod == 0b11) {
        instr.source = general_register_operand(params, true);
        instr.size = 2;
    }

    if (params.d == 0) {
        Instruction_Operand temp = instr.dest;
        instr.dest = instr.source;
        instr.source = temp;
    }

    return instr;
}

Instruction decode_segment_register_mov(FN_PARAMS) {
    Instruction instr;
    Instruction_Params params;

    uint8_t byte1 = buffer[index];
    uint8_t byte2 = buffer[index + 1];

    params.w = 1;
    params.mod = (byte2 >> 6) & 0b11;
    params.rm = byte2 & 0b111;
    uint8_t sr = (byte2 >> 3) & 0b11;

    instr.op_type = Op_mov;
    instr.dest = segment_register_operand(sr);

    if (params.mod == 0) {
        // exception here according to the manual for rm = 0b110
        if (params.rm == 0b110) {
            int16_t displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
            instr.source = effective_address_operand(params, displacement);
            instr.size = 4;
            instr.source.address.is_word = params.w;
        } else {
            instr.source = effective_address_operand(params, 0);
            instr.size = 2;
        }
    } else if (params.mod == 0b01 || params.mod == 0b10) {
        int16_t displacement = 0;
        if (params.mod == 0b01) {
            displacement = (int8_t) buffer[index+2];
            instr.source = effective_address_operand(params, displacement);
            instr.size = 3;
            instr.source.address.is_word = params.w;
        } else if (params.mod == 0b10) {
            displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
            instr.source = effective_address_operand(params, displacement);
            instr.size = 4;
            instr.source.address.is_word = params.w;
        }
    } else if (params.mod == 0b11) {
        instr.source = general_register_operand(params, true);
        instr.size = 2;
    }

    if (buffer[index] == MOV_SEG_TO_REGMEM_OPCODE) {
        Instruction_Operand temp = instr.dest;
        instr.dest = instr.source;
        instr.source = temp;
    }

    return instr;
}

Instruction decode_immediate_mov(FN_PARAMS) {
    Instruction instr;
    Instruction_Params params = {};

    uint8_t byte = buffer[index];
    params.w = (byte >> 3) & 0b1;
    params.reg = byte & 0b111;

    instr.op_type = Op_mov;
    instr.dest = general_register_operand(params, false);

    if (params.w) {
        uint16_t s = (int16_t)buffer[index+1] | ((int16_t)buffer[index+2] << 8);
        instr.source = immediate_operand(s);
        instr.size = 3;
    } else {
        uint16_t s = buffer[index+1];
        instr.source = immediate_operand(s);
        instr.size = 2;
    }

    return instr;
}

Instruction decode_immediate_arithmetic(FN_PARAMS) {
    Instruction instr;
    Instruction_Params params;

    uint8_t byte = buffer[index];
    uint8_t s = (byte >> 1) & 0b1;
    params.w = byte & 0b1;
    params.mod = (buffer[index+1] >> 6) & 0b11;
    params.rm = buffer[index+1] & 0b111;
    instr.size = 2;
    // we have to set reg to rm because in this specific isntruction we use rm for indexing, this is messy, probably should change the way Reg_Access works
    // but whatever
    params.reg = params.rm;

    int16_t displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
    int16_t data = 0;

    uint8_t op_type = (buffer[index+1] >> 3) & 0b111;
    switch (op_type) {
        case 0b000:
            instr.op_type = Op_add;
            break;
        case 0b101:
            instr.op_type = Op_sub;
            break;
        case 0b111:
            instr.op_type = Op_cmp;
            break;
    }

    int16_t data_idx = index + 2;
    switch (params.mod) {
        case 0b00:
            if (params.rm == 0b110) {
                instr.dest = effective_address_operand(params, displacement);
                data_idx = index + 4;
                instr.size += 2;
                instr.source.address.is_word = params.w;
            } else {
                instr.dest = effective_address_operand(params, 0);
                data_idx = index + 2;
                instr.source.address.is_word = params.w;
            }
            break;
        case 0b01:
            instr.dest = effective_address_operand(params, displacement);
            instr.size++;
            data_idx = index + 3;
            instr.source.address.is_word = params.w;
            break;
        case 0b10:
            instr.dest = effective_address_operand(params, displacement);
            instr.size += 2;
            data_idx = index + 4;
            instr.source.address.is_word = params.w;
            break;
        case 0b11:
            instr.dest = general_register_operand(params, false);
            data_idx = index + 2;
            break;
        default:
            printf("[ERROR]: decode_immediate_arithmetic, mod is not 0b00, 0b01, 0b10 or 0b11, mod: %i\n", params.mod);
    }

    if (params.w) {
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

    instr.source = immediate_operand(data);

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
            instr.op_type = Op_add;
            break;
        case SUB_OPCODE:
            instr.op_type = Op_sub;
            break;
        case CMP_OPCODE:
            instr.op_type = Op_cmp;
            break;
    }

    instr.dest = general_register_operand(params, false);

    if (params.mod == 0) {
        // exception here according to the manual for rm = 0b110
        if (params.rm == 0b110) {
            int16_t displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
            instr.source = effective_address_operand(params, displacement);
            instr.size = 4;
            instr.source.address.is_word = params.w;
        } else {
            instr.source = effective_address_operand(params, 0);
            instr.size = 2;
        }
    } else if (params.mod == 0b01 || params.mod == 0b10) {
        int16_t displacement = 0;
        if (params.mod == 0b01) {
            displacement = (int8_t) buffer[index+2];
            instr.size = 3;
        } else if (params.mod == 0b10) {
            displacement = (int16_t)buffer[index+2] | ((int16_t)buffer[index+3] << 8);
            instr.size = 4;
        }

        instr.source = effective_address_operand(params, displacement);
        instr.source.address.is_word = params.w;
    } else if (params.mod == 0b11) {
        instr.source = general_register_operand(params, true);
        instr.size = 2;
    }

    if (params.d == 0) {
        Instruction_Operand temp = instr.dest;
        instr.dest = instr.source;
        instr.source = temp;
    }

    return instr;
}

Instruction decode_immediate_acumm(FN_PARAMS) {
    Instruction instr;
    Instruction_Params params = {};
    uint8_t byte = buffer[index];
    params.w = byte & 0b1;
    params.reg = 0;
    int16_t data = 0;

    instr.dest = general_register_operand(params, false);

    uint8_t opcode = byte & IMMD_TO_ACUMM_MASK;
    if (opcode == IMMD_TO_ACUMM_ADD_OPCODE) {
        instr.op_type = Op_mov;
    } else if (opcode == IMMD_TO_ACUMM_SUB_OPCODE) {
        instr.op_type = Op_sub;
    } else if (opcode == IMMD_TO_ACUMM_CMP_OPCODE) {
        instr.op_type = Op_sub;
    }

    if (params.w) {
        data = (int16_t)buffer[index+1] | ((int16_t)buffer[index+2] << 8);
        instr.size = 3;
    } else {
        data = (int8_t)buffer[index+1];
        instr.size = 2;
    }

    instr.source = immediate_operand(data);

    return instr;
}

Instruction decode_cond_jump(FN_PARAMS) {
    Instruction instr;
    uint8_t byte = buffer[index];
    int8_t offset = (int8_t) buffer[index + 1];
    instr.op_type = get_cond_jump_type(byte);
    instr.dest = immediate_operand(offset);
    instr.source = none_operand();
    instr.size = 2;

    return instr;
}

Instruction decode_control_transfer(FN_PARAMS) {
    Instruction instr;
    uint8_t byte = buffer[index];
    int8_t offset = buffer[index+1];
    instr.source = none_operand();

    if (byte == LOOP_OPCODE) {
        instr.op_type = Op_loop;
    } else if (byte == LOOPZ_OPCODE) {
        instr.op_type = Op_loopz;
    } else if (byte == LOOPNZ_OPCODE) {
        instr.op_type = Op_loopnz;
    } else if (byte == JCXZ_OPCODE) {
        instr.op_type = Op_jcxz;
    }

    instr.dest = immediate_operand(offset);
    instr.size = 2;

    return instr;
}
