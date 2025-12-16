#pragma once

#include <stdint.h>
#include <assert.h>
#include "memory.h"

#define REG_COUNT 14
typedef union {
    #define REG_16(i) union {struct{uint8_t i##l; uint8_t i##h;}; uint16_t i##x;}
    struct {
        REG_16(a);
        REG_16(b);
        REG_16(c);
        REG_16(d);
        uint16_t sp;
        uint16_t bp;
        uint16_t si;
        uint16_t di;
        uint16_t es;
        uint16_t cs;
        uint16_t ss;
        uint16_t ds;
        uint16_t ip;
        uint16_t flags;
    };
    uint8_t reg8[REG_COUNT][2];
    uint16_t reg16[REG_COUNT];

    #undef REG_16
} Regs_8086;
#define FLAGS_REGISTER_8086 14
static_assert((sizeof(Regs_8086) / sizeof(uint16_t) == REG_COUNT), "Mismatched register sizes");

struct Instruction;

void execute_instructions(Seg_Mem *main_memory, Seg_Mem *instructions_mem);
static void execute_instruction(Seg_Mem memory, Regs_8086 *regs, struct Instruction instruction);
