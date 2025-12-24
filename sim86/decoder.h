#pragma once

#include <stdint.h>
#include "instruction.h"

#define MOV_OPCODE 0b10001000
#define MOV_MASK   0b11111100

#define MOV_REGMEM_TO_SEG_OPCODE 0b10001110
#define MOV_REGMEM_TO_SEG_MASK 0b11111111

#define MOV_SEG_TO_REGMEM_OPCODE 0b10001100
#define MOV_SEG_TO_REGMEM_MASK 0b11111111

#define IMMD_OP_OPCODE 0b10110000
#define IMMD_OP_MASK   0b11110000

#define IMMD_OP_ARITHMETIC_MASK   0b11111100
#define IMMD_OP_ARITHMETIC_OPCODE 0b10000000

#define ARITHMETIC_MASK 0b11111100
#define ADD_MASK        0b11111100
#define ADD_OPCODE      0b00000000
#define SUB_MASK        0b11111100
#define SUB_OPCODE      0b00101000
#define CMP_MASK        0b11111100
#define CMP_OPCODE      0b00111000

#define IMMD_TO_ACUMM_MASK        0b11111110
#define IMMD_TO_ACUMM_ADD_OPCODE  0b00000100
#define IMMD_TO_ACUMM_SUB_OPCODE  0b00101100
#define IMMD_TO_ACUMM_CMP_OPCODE  0b00111100

#define COND_JUMP_INIT_INDEX 112
#define COND_JUMP_INDEX(opcode) (opcode - COND_JUMP_INIT_INDEX)
#define COND_JUMP_MASK 0b11111111
#define JO_OPCODE    0b01110000
#define JNO_OPCODE   0b01110001
#define JB_OPCODE    0b01110010
#define JBE_OPCODE   0b01110110
#define JNB_OPCODE   0b01110011
#define JE_OPCODE    0b01110100
#define JNE_OPCODE   0b01110101
#define JA_OPCODE    0b01110111
#define JS_OPCODE    0b01111000
#define JNS_OPCODE   0b01111001
#define JP_OPCODE    0b01111010
#define JNP_OPCODE   0b01111011
#define JL_OPCODE    0b01111100
#define JNL_OPCODE   0b01111101
#define JLE_OPCODE   0b01111110
#define JG_OPCODE    0b01111111

#define LOOP_MASK 0b11111111
#define LOOP_OPCODE 0b11100010

#define LOOPZ_MASK 0b11111111
#define LOOPZ_OPCODE 0b11100001

#define LOOPNZ_MASK 0b11111111
#define LOOPNZ_OPCODE 0b11100000

#define JCXZ_MASK 0b11111111
#define JCXZ_OPCODE 0b11100011

#define FN_PARAMS uint8_t *buffer, uint16_t index

typedef struct {
    uint16_t mask;
    uint16_t opcode;
    Instruction (*decode_fn)(uint8_t*, uint16_t);
} Opcode_Pattern;

extern Opcode_Pattern opcode_patterns[31];

static uint32_t cond_jump_table[16];
Instruction decode_instruction(FN_PARAMS);
Instruction default_instruction(FN_PARAMS);
Instruction decode_mov(FN_PARAMS);
Instruction decode_segment_register_mov(FN_PARAMS);
Instruction decode_immediate_mov(FN_PARAMS);
Instruction decode_immediate_arithmetic(FN_PARAMS);
Instruction decode_arithmetic(FN_PARAMS);
Instruction decode_immediate_acumm(FN_PARAMS);
Instruction decode_cond_jump(FN_PARAMS);
Instruction decode_control_transfer(FN_PARAMS);

Operation_Type get_cond_jump_type(uint8_t opcode);
