#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "memory.h"
#include "instruction.h"

#define SEGMENT_REGISTER_START 8

#define REG_COUNT 14
typedef union {
    #define REG_16(i) union {struct{uint8_t i##l; uint8_t i##h;}; uint16_t i##x;}
    struct {
        REG_16(a);
        REG_16(c);
        REG_16(d);
        REG_16(b);
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
    uint8_t reg8[REG_COUNT * 2];
    uint16_t reg16[REG_COUNT];

    #undef REG_16
} Regs_8086;
#define FLAGS_REGISTER_8086 14
static_assert((sizeof(Regs_8086) / sizeof(uint16_t) == REG_COUNT), "Mismatched register sizes");

#define SET_FLAG(flag) registers_state.flags |= flag;
#define UNSET_FLAG(flag) registers_state.flags &= ~flag;
#define UPDATE_FLAG(mask, cond) \
    (registers_state.flags = \
        (registers_state.flags & ~(mask)) | ((cond) ? (mask) : 0)) \

#define FLAG_CF (1u << 0)
#define FLAG_PF (1u << 2)
#define FLAG_AF (1u << 4)
#define FLAG_ZF (1u << 6)
#define FLAG_SF (1u << 7)
#define FLAG_TF (1u << 8)
#define FLAG_IF (1u << 9)
#define FLAG_DF (1u << 10)
#define FLAG_OF (1u << 11)

extern Regs_8086 registers_state;
static Effective_Address_Operand effective_address_expr_table[8][2];

static void check_common_flags(Seg_Mem *main_memory, Instruction_Operand result);
static uint16_t get_signed_bit(uint16_t size);

static void mov(Seg_Mem *main_memory, Instruction instruction);
static void add(Seg_Mem *main_memory, Instruction instruction);
static void sub(Seg_Mem *main_memory, Instruction instruction);
static void cmp(Seg_Mem *main_memory, Instruction instruction);
static void conditional_jump(Seg_Mem *main_memory, int8_t displacement, bool should_jump);
static void jump(int8_t displacement);

static uint16_t get_operand_value(Seg_Mem *main_memory, Instruction_Operand operand);
static void set_operand_value(Seg_Mem *main_memory, Instruction_Operand operand, uint16_t val);

static Seg_Mem get_operand_ref(Seg_Mem *main_memory, Instruction_Operand operand);
static Seg_Mem get_register_ref(Reg_Access reg_access);

uint16_t calculate_effective_address(Effective_Address_Expression expr);

void execute_instructions(Seg_Mem *main_memory, Seg_Mem *instructions_mem);
Operand_State execute_instruction(Seg_Mem *main_memory, Instruction instruction);

void print_registers_state();
void print_real_registers_state();
void print_flags_state();
